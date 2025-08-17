/**
 * @file bme280.h
 * @brief Header file for the BME280 sensor library
 *
 * This library provides functions to interface with the Bosch BME280 sensor,
 * which measures temperature, pressure, and humidity. Designed for use with
 * the bcm2835 library on Raspberry Pi for I2C communication, as part of the
 * Pwnsat LoRa Packet Analyzer project.
 *
 * @author Pwnsat Team
 * @date 2025-08-13
 * @license SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef BME280_H
#define BME280_H
#ifdef __cplusplus
extern "C" {
#endif

#include "i2c_tools.h"

/** @brief Default I2C address for BME280 */
#define BME280_ADDRESS (0x77)

/** @brief Alternate I2C address for BME280 */
#define BME280_ADDRESS_ALTERNATE (0x76)

/** @brief Standard sea-level pressure in hPa */
#define SEALEVELPRESSURE_HPA (1013.25)

/**
 * @brief Register addresses for BME280
 */
enum {
  BME280_REGISTER_DIG_T1 = 0x88,    /**< Temperature calibration register 1 */
  BME280_REGISTER_DIG_T2 = 0x8A,    /**< Temperature calibration register 2 */
  BME280_REGISTER_DIG_T3 = 0x8C,    /**< Temperature calibration register 3 */
  BME280_REGISTER_DIG_P1 = 0x8E,    /**< Pressure calibration register 1 */
  BME280_REGISTER_DIG_P2 = 0x90,    /**< Pressure calibration register 2 */
  BME280_REGISTER_DIG_P3 = 0x92,    /**< Pressure calibration register 3 */
  BME280_REGISTER_DIG_P4 = 0x94,    /**< Pressure calibration register 4 */
  BME280_REGISTER_DIG_P5 = 0x96,    /**< Pressure calibration register 5 */
  BME280_REGISTER_DIG_P6 = 0x98,    /**< Pressure calibration register 6 */
  BME280_REGISTER_DIG_P7 = 0x9A,    /**< Pressure calibration register 7 */
  BME280_REGISTER_DIG_P8 = 0x9C,    /**< Pressure calibration register 8 */
  BME280_REGISTER_DIG_P9 = 0x9E,    /**< Pressure calibration register 9 */
  BME280_REGISTER_DIG_H1 = 0xA1,    /**< Humidity calibration register 1 */
  BME280_REGISTER_DIG_H2 = 0xE1,    /**< Humidity calibration register 2 */
  BME280_REGISTER_DIG_H3 = 0xE3,    /**< Humidity calibration register 3 */
  BME280_REGISTER_DIG_H4 = 0xE4,    /**< Humidity calibration register 4 */
  BME280_REGISTER_DIG_H5 = 0xE5,    /**< Humidity calibration register 5 */
  BME280_REGISTER_DIG_H6 = 0xE7,    /**< Humidity calibration register 6 */
  BME280_REGISTER_CHIPID = 0xD0,    /**< Chip ID register */
  BME280_REGISTER_VERSION = 0xD1,   /**< Version register */
  BME280_REGISTER_SOFTRESET = 0xE0, /**< Soft reset register */
  BME280_REGISTER_CAL26 = 0xE1,     /**< Calibration data (0xE1-0xF0) */
  BME280_REGISTER_CONTROLHUMID = 0xF2, /**< Humidity control register */
  BME280_REGISTER_STATUS = 0xF3,       /**< Status register */
  BME280_REGISTER_CONTROL = 0xF4,      /**< Control register */
  BME280_REGISTER_CONFIG = 0xF5,       /**< Configuration register */
  BME280_REGISTER_PRESSUREDATA = 0xF7, /**< Pressure data register */
  BME280_REGISTER_TEMPDATA = 0xFA,     /**< Temperature data register */
  BME280_REGISTER_HUMIDDATA = 0xFD     /**< Humidity data register */
};

/**
 * @brief Structure to store BME280 calibration data
 */
typedef struct {
  uint16_t dig_T1; /**< Temperature compensation value */
  int16_t dig_T2;  /**< Temperature compensation value */
  int16_t dig_T3;  /**< Temperature compensation value */
  uint16_t dig_P1; /**< Pressure compensation value */
  int16_t dig_P2;  /**< Pressure compensation value */
  int16_t dig_P3;  /**< Pressure compensation value */
  int16_t dig_P4;  /**< Pressure compensation value */
  int16_t dig_P5;  /**< Pressure compensation value */
  int16_t dig_P6;  /**< Pressure compensation value */
  int16_t dig_P7;  /**< Pressure compensation value */
  int16_t dig_P8;  /**< Pressure compensation value */
  int16_t dig_P9;  /**< Pressure compensation value */
  uint8_t dig_H1;  /**< Humidity compensation value */
  int16_t dig_H2;  /**< Humidity compensation value */
  uint8_t dig_H3;  /**< Humidity compensation value */
  int16_t dig_H4;  /**< Humidity compensation value */
  int16_t dig_H5;  /**< Humidity compensation value */
  int8_t dig_H6;   /**< Humidity compensation value */
} bme280_calib_data_t;

/**
 * @brief Sampling rates for sensor measurements
 */
enum sensor_sampling {
  SAMPLING_NONE = 0, /**< No sampling */
  SAMPLING_X1 = 1,   /**< 1x sampling */
  SAMPLING_X2 = 2,   /**< 2x sampling */
  SAMPLING_X4 = 3,   /**< 4x sampling */
  SAMPLING_X8 = 4,   /**< 8x sampling */
  SAMPLING_X16 = 5   /**< 16x sampling */
};

/**
 * @brief Power modes for the BME280
 */
enum sensor_mode {
  MODE_SLEEP = 0,  /**< Sleep mode */
  MODE_FORCED = 1, /**< Forced mode */
  MODE_NORMAL = 2  /**< Normal mode */
};

/**
 * @brief Filter settings for the BME280
 */
enum sensor_filter {
  FILTER_OFF = 0, /**< Filter off */
  FILTER_X2 = 1,  /**< 2x filter */
  FILTER_X4 = 2,  /**< 4x filter */
  FILTER_X8 = 3,  /**< 8x filter */
  FILTER_X16 = 4  /**< 16x filter */
};

/**
 * @brief Standby duration in normal mode (ms)
 */
enum standby_duration {
  STANDBY_MS_0_5 = 0,  /**< 0.5 ms */
  STANDBY_MS_10 = 6,   /**< 10 ms */
  STANDBY_MS_20 = 7,   /**< 20 ms */
  STANDBY_MS_62_5 = 1, /**< 62.5 ms */
  STANDBY_MS_125 = 2,  /**< 125 ms */
  STANDBY_MS_250 = 3,  /**< 250 ms */
  STANDBY_MS_500 = 4,  /**< 500 ms */
  STANDBY_MS_1000 = 5  /**< 1000 ms */
};

/**
 * @brief Configuration register structure
 */
struct config_reg {
  unsigned int t_sb : 3;     /**< Standby time in normal mode */
  unsigned int filter : 3;   /**< Filter settings */
  unsigned int none : 1;     /**< Unused bit */
  unsigned int spi3w_en : 1; /**< SPI 3-wire enable */
};
extern struct config_reg configReg;

/**
 * @brief Control measurement register structure
 */
struct ctrl_meas {
  unsigned int osrs_t : 3; /**< Temperature oversampling */
  unsigned int osrs_p : 3; /**< Pressure oversampling */
  unsigned int mode : 2;   /**< Device mode */
};
extern struct ctrl_meas meas_reg;

/**
 * @brief Control humidity register structure
 */
struct ctrl_hum {
  unsigned int none : 5;   /**< Unused bits */
  unsigned int osrs_h : 3; /**< Humidity oversampling */
};
extern struct ctrl_hum hum_reg;

/**
 * @brief Initialize and configure the BME280 sensor
 * @param slave I2C address of the sensor (0x76 or 0x77)
 * @return 0 on success, negative value on error
 */
int bme280_begin(uint8_t slave);

/**
 * @brief Read compensated temperature from the BME280
 * @return Temperature in degrees Celsius (°C)
 */
float bme280_read_temperature(void);

/**
 * @brief Read compensated pressure from the BME280
 * @return Pressure in hectopascals (hPa)
 */
float bme280_read_pressure(void);

/**
 * @brief Calculate altitude based on pressure
 * @param seaLevel Sea-level pressure in hPa (default: 1013.25)
 * @return Altitude in meters
 */
float bme280_read_altitude(float seaLevel);

/**
 * @brief Read compensated humidity from the BME280
 * @return Relative humidity in percentage (%)
 */
float bme280_read_humidity(void);

#ifdef __cplusplus
}
#endif
#endif // BME280_H