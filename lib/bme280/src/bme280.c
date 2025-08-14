/**
 * @file bme280.c
 * @brief Implementation of the BME280 sensor library
 *
 * This library interfaces with the Bosch BME280 sensor to read temperature,
 * pressure, and humidity. It uses the bcm2835 library for I2C communication
 * and is part of the Pwnsat LoRa Packet Analyzer project, potentially for
 * cubesat or IoT applications.
 *
 * @author Pwnsat Team
 * @date 2025-08-13
 * @license GNU General Public License v3.0
 */

#include <bme280.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

/** @brief Calibration data for the BME280 */
static bme280_calib_data_t bme280_calib;

/** @brief Fine temperature value used for compensation calculations */
static int32_t t_fine;

/** @brief Adjustment for temperature readings (affects pressure and humidity)
 */
static int32_t t_fine_adjust = 0;

/** @brief Global configuration registers */
struct config_reg configReg;
struct ctrl_meas meas_reg;
struct ctrl_hum hum_reg;

static uint8_t slave_addr = 0x0;

/**
 * @brief Perform a soft reset of the BME280
 * @return 0 on success, negative value on error
 */
static int bme280_reset(void) {
  i2c_tools_set_slave_address(slave_addr);
  // Write 0xB6 to the soft reset register (0xE0)
  int ret = i2c_tool_write_reg(BME280_REGISTER_SOFTRESET, 0xB6);
  if (ret != 0) {
    fprintf(stderr, "Error resetting BME280: %d\n", ret);
    return ret;
  }
  return 0;
}

/**
 * @brief Check if the BME280 is in calibration mode
 * @return Non-zero if calibration is in progress, 0 otherwise
 */
static int bme280_in_calibration(void) {
  i2c_tools_set_slave_address(slave_addr);
  uint8_t buffer = i2c_tool_read_byte(BME280_REGISTER_STATUS);
  return (buffer & (1 << 0)) != 0;
}

/**
 * @brief Read calibration coefficients from the BME280
 */
static void bme280_read_coefficients(void) {
  i2c_tools_set_slave_address(slave_addr);
  bme280_calib.dig_T1 = i2c_tool_read16_le(BME280_REGISTER_DIG_T1);
  bme280_calib.dig_T2 = i2c_tool_reads16_le(BME280_REGISTER_DIG_T2);
  bme280_calib.dig_T3 = i2c_tool_reads16_le(BME280_REGISTER_DIG_T3);

  bme280_calib.dig_P1 = i2c_tool_read16_le(BME280_REGISTER_DIG_P1);
  bme280_calib.dig_P2 = i2c_tool_reads16_le(BME280_REGISTER_DIG_P2);
  bme280_calib.dig_P3 = i2c_tool_reads16_le(BME280_REGISTER_DIG_P3);
  bme280_calib.dig_P4 = i2c_tool_reads16_le(BME280_REGISTER_DIG_P4);
  bme280_calib.dig_P5 = i2c_tool_reads16_le(BME280_REGISTER_DIG_P5);
  bme280_calib.dig_P6 = i2c_tool_reads16_le(BME280_REGISTER_DIG_P6);
  bme280_calib.dig_P7 = i2c_tool_reads16_le(BME280_REGISTER_DIG_P7);
  bme280_calib.dig_P8 = i2c_tool_reads16_le(BME280_REGISTER_DIG_P8);
  bme280_calib.dig_P9 = i2c_tool_reads16_le(BME280_REGISTER_DIG_P9);

  bme280_calib.dig_H1 = i2c_tool_read_byte(BME280_REGISTER_DIG_H1);
  bme280_calib.dig_H2 = i2c_tool_reads16_le(BME280_REGISTER_DIG_H2);
  bme280_calib.dig_H3 = i2c_tool_read_byte(BME280_REGISTER_DIG_H3);
  bme280_calib.dig_H4 =
      ((int8_t)i2c_tool_read_byte(BME280_REGISTER_DIG_H4) << 4) |
      (i2c_tool_read_byte(BME280_REGISTER_DIG_H4 + 1) & 0xF);
  bme280_calib.dig_H5 =
      ((int8_t)i2c_tool_read_byte(BME280_REGISTER_DIG_H5 + 1) << 4) |
      (i2c_tool_read_byte(BME280_REGISTER_DIG_H5) >> 4);
  bme280_calib.dig_H6 = (int8_t)i2c_tool_read_byte(BME280_REGISTER_DIG_H6);
}

/**
 * @brief Configure sampling settings for the BME280
 */
static void bme280_set_sampling(void) {
  i2c_tools_set_slave_address(slave_addr);
  meas_reg.mode = MODE_NORMAL;
  meas_reg.osrs_t = SAMPLING_X16;
  meas_reg.osrs_p = SAMPLING_X16;
  hum_reg.osrs_h = SAMPLING_X16;
  configReg.filter = FILTER_OFF;
  configReg.t_sb = STANDBY_MS_0_5;
  configReg.spi3w_en = 0;

  // Ensure sensor is in sleep mode before setting configuration
  i2c_tool_write_reg(BME280_REGISTER_CONTROL, MODE_SLEEP);

  // Write configuration registers (CONTROLHUMID must be set before CONTROL)
  uint8_t hum_data = hum_reg.osrs_h;
  uint8_t config_data =
      (configReg.t_sb << 5) | (configReg.filter << 2) | configReg.spi3w_en;
  uint8_t meas_data =
      (meas_reg.osrs_t << 5) | (meas_reg.osrs_p << 2) | meas_reg.mode;

  i2c_tool_write_reg(BME280_REGISTER_CONTROLHUMID, hum_data);
  i2c_tool_write_reg(BME280_REGISTER_CONFIG, config_data);
  i2c_tool_write_reg(BME280_REGISTER_CONTROL, meas_data);
}

/**
 * @brief Read and compensate temperature from the BME280
 * @return Temperature in degrees Celsius (°C)
 */
float bme280_read_temperature(void) {
  i2c_tools_set_slave_address(slave_addr);
  int32_t var1, var2;
  if (meas_reg.osrs_t == SAMPLING_NONE) {
    return 0;
  }

  int32_t adc_T = i2c_tool_read24(BME280_REGISTER_TEMPDATA);
  adc_T >>= 4;

  var1 = (int32_t)((adc_T / 8) - ((int32_t)bme280_calib.dig_T1 * 2));
  var1 = (var1 * ((int32_t)bme280_calib.dig_T2)) / 2048;
  var2 = (int32_t)((adc_T / 16) - ((int32_t)bme280_calib.dig_T1));
  var2 = (((var2 * var2) / 4096) * ((int32_t)bme280_calib.dig_T3)) / 16384;

  t_fine = var1 + var2 + t_fine_adjust;
  int32_t T = (t_fine * 5 + 128) / 256;

  return (float)T / 100;
}

/**
 * @brief Read and compensate pressure from the BME280
 * @return Pressure in hectopascals (hPa)
 */
float bme280_read_pressure(void) {
  i2c_tools_set_slave_address(slave_addr);
  int64_t var1, var2, var3, var4;
  if (meas_reg.osrs_p == SAMPLING_NONE) {
    return 0;
  }

  bme280_read_temperature(); // Required to update t_fine

  int32_t adc_P = i2c_tool_read24(BME280_REGISTER_PRESSUREDATA);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)bme280_calib.dig_P6;
  var2 = var2 + ((var1 * (int64_t)bme280_calib.dig_P5) * 131072);
  var2 = var2 + (((int64_t)bme280_calib.dig_P4) * 34359738368);
  var1 = ((var1 * var1 * (int64_t)bme280_calib.dig_P3) / 256) +
         ((var1 * ((int64_t)bme280_calib.dig_P2) * 4096));
  var3 = ((int64_t)1) * 140737488355328;
  var1 = (var3 + var1) * ((int64_t)bme280_calib.dig_P1) / 8589934592;

  if (var1 == 0) {
    return 0; // Avoid division by zero
  }

  var4 = 1048576 - adc_P;
  var4 = (((var4 * 2147483648) - var2) * 3125) / var1;
  var1 = (((int64_t)bme280_calib.dig_P9) * (var4 / 8192) * (var4 / 8192)) /
         33554432;
  var2 = (((int64_t)bme280_calib.dig_P8) * var4) / 524288;
  var4 = ((var4 + var1 + var2) / 256) + (((int64_t)bme280_calib.dig_P7) * 16);

  return (float)var4 / 256.0;
}

/**
 * @brief Calculate altitude based on pressure
 * @param seaLevel Sea-level pressure in hPa (default: 1013.25)
 * @return Altitude in meters
 */
float bme280_read_altitude(float seaLevel) {
  i2c_tools_set_slave_address(slave_addr);
  float atmospheric = bme280_read_pressure() / 100.0F;
  return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}

/**
 * @brief Read and compensate humidity from the BME280
 * @return Relative humidity in percentage (%)
 */
float bme280_read_humidity(void) {
  i2c_tools_set_slave_address(slave_addr);
  int32_t var1, var2, var3, var4, var5;
  if (hum_reg.osrs_h == SAMPLING_NONE) {
    return 0;
  }

  bme280_read_temperature(); // Required to update t_fine

  int32_t adc_H = i2c_tool_read16(BME280_REGISTER_HUMIDDATA);
  var1 = t_fine - ((int32_t)76800);
  var2 = (int32_t)(adc_H * 16384);
  var3 = (int32_t)(((int32_t)bme280_calib.dig_H4) * 1048576);
  var4 = ((int32_t)bme280_calib.dig_H5) * var1;
  var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
  var2 = (var1 * ((int32_t)bme280_calib.dig_H6)) / 1024;
  var3 = (var1 * ((int32_t)bme280_calib.dig_H3)) / 2048;
  var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
  var2 = ((var4 * ((int32_t)bme280_calib.dig_H2)) + 8192) / 16384;
  var3 = var5 * var2;
  var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
  var5 = var3 - ((var4 * ((int32_t)bme280_calib.dig_H1)) / 16);
  var5 = (var5 < 0 ? 0 : var5);
  var5 = (var5 > 419430400 ? 419430400 : var5);
  uint32_t H = (uint32_t)(var5 / 4096);

  return (float)H / 1024.0;
}

/**
 * @brief Initialize the BME280 sensor
 * @param slave I2C address of the sensor (0x76 or 0x77)
 * @return 0 on success, negative value on error
 */
static int bme280_init(uint8_t slave) {
  int ret = i2c_tools_init();
  if (ret != BCM2835_I2C_REASON_OK) {
    fprintf(stderr, "Error initializing I2C: %d\n", ret);
    return ret;
  }
  ret = i2c_tools_set_slave_address(slave);
  if (ret != BCM2835_I2C_REASON_OK) {
    fprintf(stderr, "Error starting I2C with slave 0x%02X: %d\n", slave, ret);
    return ret;
  }
  slave_addr = slave;
  ret = bme280_reset();
  if (ret != 0) {
    fprintf(stderr, "Error resetting BME280: %d\n", ret);
    return ret;
  }
  bcm2835_delay(2); // Delay for reset (~2ms per datasheet)

  uint8_t chip_id = i2c_tool_read_byte(BME280_REGISTER_CHIPID);
  if (chip_id == 0x60) {
    printf("Success: BME280 detected\n");
  } else if (chip_id == 0x58) {
    printf("Success: BMP280 detected\n");
  } else {
    printf("Warning: Expected chip ID 0x60 or 0x58, got 0x%02X\n", chip_id);
    return -1;
  }
  return 0;
}

/**
 * @brief Initialize and configure the BME280 sensor
 * @param slave I2C address of the sensor (0x76 or 0x77)
 * @return 0 on success, negative value on error
 */
int bme280_begin(uint8_t slave) {
  if (bme280_init(slave) != 0) {
    fprintf(stderr, "Error initializing BME280\n");
    return -1;
  }

  while (bme280_in_calibration() != 0) {
    bcm2835_delay(10);
  }

  bme280_read_coefficients();
  bme280_set_sampling();
  bcm2835_delay(100);
  return 0;
}