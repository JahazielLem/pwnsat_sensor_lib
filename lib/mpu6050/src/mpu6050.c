#include "mpu6050.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

static uint8_t slave_addr = 0x0;

#define BIT0 (1 << 0) // 0x01
#define BIT1 (1 << 1) // 0x02
#define BIT2 (1 << 2) // 0x04
#define BIT3 (1 << 3) // 0x08
#define BIT4 (1 << 4) // 0x10
#define BIT5 (1 << 5) // 0x20
#define BIT6 (1 << 6) // 0x40
#define BIT7 (1 << 7) // 0x80

void mpu6050_get_raw_gyro(mpu6050_raw_gyro_value_t *raw_gyro_value) {
  char buffer[6] = {0};
  i2c_tools_read_reg(MPU6050_GYRO_XOUT_H, buffer, 6);

  raw_gyro_value->raw_gyro_x =
      (int16_t)(((uint8_t)buffer[0] << 8) + ((uint8_t)buffer[1]));
  raw_gyro_value->raw_gyro_y =
      (int16_t)(((uint8_t)buffer[2] << 8) + ((uint8_t)buffer[3]));
  raw_gyro_value->raw_gyro_z =
      (int16_t)(((uint8_t)buffer[4] << 8) + ((uint8_t)buffer[5]));
}

void mpu6050_get_raw_acce(mpu6050_raw_acce_value_t *raw_acce_value) {
  char buffer[6] = {0};
  i2c_tools_read_reg(MPU6050_ACCEL_XOUT_H, buffer, 6);

  raw_acce_value->raw_acce_x =
      (int16_t)(((uint8_t)buffer[0] << 8) + ((uint8_t)buffer[1]));
  raw_acce_value->raw_acce_y =
      (int16_t)(((uint8_t)buffer[2] << 8) + ((uint8_t)buffer[3]));
  raw_acce_value->raw_acce_z =
      (int16_t)(((uint8_t)buffer[4] << 8) + ((uint8_t)buffer[5]));
}

float mpu6050_get_acce_sensitivity(void) {
  i2c_tools_set_slave_address(slave_addr);
  float acce_sensitivity = 0;
  uint8_t acce_fs = i2c_tool_read_byte(MPU6050_ACCEL_CONFIG);
  acce_fs = (acce_fs >> 3) & 0x03;
  switch (acce_fs) {
  case MPU6050_RANGE_2_G:
    acce_sensitivity = 16384;
    break;
  case MPU6050_RANGE_4_G:
    acce_sensitivity = 8192;
    break;
  case MPU6050_RANGE_8_G:
    acce_sensitivity = 4096;
    break;
  case MPU6050_RANGE_16_G:
    acce_sensitivity = 2048;
    break;
  default:
    break;
  }
  return acce_sensitivity;
}

float mpu6050_get_gyro_sensitivity(void) {
  i2c_tools_set_slave_address(slave_addr);
  float gyro_sensitivity = 0;
  uint8_t gyro_fs = i2c_tool_read_byte(MPU6050_ACCEL_CONFIG);
  gyro_fs = (gyro_fs >> 3) & 0x03;
  switch (gyro_fs) {
  case MPU6050_RANGE_250_DEG:
    gyro_sensitivity = 131;
    break;
  case MPU6050_RANGE_500_DEG:
    gyro_sensitivity = 65.5;
    break;
  case MPU6050_RANGE_1000_DEG:
    gyro_sensitivity = 32.8;
    break;
  case MPU6050_RANGE_2000_DEG:
    gyro_sensitivity = 16.4;
    break;
  default:
    break;
  }
  return gyro_sensitivity;
}

int mpu6050_get_gyro(mpu6050_gyro_value_t *gyro_value) {
  mpu6050_raw_gyro_value_t raw_gyro;

  float gyro_sensitivity = mpu6050_get_gyro_sensitivity();
  mpu6050_get_raw_gyro(&raw_gyro);

  gyro_value->gyro_x = raw_gyro.raw_gyro_x / gyro_sensitivity;
  gyro_value->gyro_y = raw_gyro.raw_gyro_y / gyro_sensitivity;
  gyro_value->gyro_z = raw_gyro.raw_gyro_z / gyro_sensitivity;
  return 0;
}

int mpu6050_get_acce(mpu6050_acce_value_t *acce_value) {
  mpu6050_raw_acce_value_t raw_acce;

  float acce_sensitivity = mpu6050_get_acce_sensitivity();
  mpu6050_get_raw_acce(&raw_acce);

  acce_value->acce_x = raw_acce.raw_acce_x / acce_sensitivity;
  acce_value->acce_y = raw_acce.raw_acce_y / acce_sensitivity;
  acce_value->acce_z = raw_acce.raw_acce_z / acce_sensitivity;
  return 0;
}

int mpu6050_set_gyro_fs(mpu6050_gyro_range_t gyro_fs) {
  i2c_tools_set_slave_address(slave_addr);
  uint8_t tmp = i2c_tool_read_byte(MPU6050_GYRO_CONFIG);

  // Set the bit 3 and 4 to put in select
  tmp &= (~BIT3);
  tmp &= (~BIT4);
  tmp |= (gyro_fs << 3);
  return i2c_tool_write_reg(MPU6050_GYRO_CONFIG, tmp);
}

int mpu6050_set_acce_fs(mpu6050_accel_range_t acce_fs) {
  i2c_tools_set_slave_address(slave_addr);
  uint8_t tmp = i2c_tool_read_byte(MPU6050_ACCEL_CONFIG);

  // Set the bit 3 and 4 to put in select
  tmp &= (~BIT3);
  tmp &= (~BIT4);
  tmp |= (acce_fs << 3);
  return i2c_tool_write_reg(MPU6050_ACCEL_CONFIG, tmp);
}

int mpu6050_wake_up(void) {
  i2c_tools_set_slave_address(slave_addr);
  uint8_t tmp = i2c_tool_read_byte(MPU6050_PWR_MGMT_1);

  // Set the bit 6 to wake up
  tmp &= (~BIT6);
  return i2c_tool_write_reg(MPU6050_PWR_MGMT_1, tmp);
}

static int mpu6050_init(uint8_t slave) {
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
  uint8_t chip_id = i2c_tool_read_byte(MPU6050_WHO_AM_I);
  if (chip_id == 0x68) {
    printf("Success: MPU6050 detected\n");
  } else if (chip_id == 0x58) {
    printf("Success: BMP280 detected\n");
  } else {
    printf("Warning: Expected chip ID 0x60 or 0x58, got 0x%02X\n", chip_id);
    return -1;
  }
  return 0;
}

int mpu6050_begin(uint8_t slave) {
  if (mpu6050_init(slave) != 0) {
    fprintf(stderr, "Error initializing MPU6050\n");
    return -1;
  }

  mpu6050_wake_up();
  return 0;
}