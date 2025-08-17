/**
 * @file mpu6050.h
 * @brief Header file for the MPU6050 sensor library
 *
 * @author Pwnsat Team
 * @date 2025-08-13
 * @license SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef MPU6050_H
#define MPU6050_H
#ifdef __cplusplus
extern "C" {
#endif

#include "i2c_tools.h"

/** @brief Default I2C address for MPU6050 */
#define MPU6050_ADDRESS (0x68) ///< MPU6050 default i2c address w/ AD0 high

enum {
  MPU6050_SELF_TEST_X = 0x0D,
  MPU6050_SELF_TEST_Y = 0x0E,
  MPU6050_SELF_TEST_Z = 0x0F,
  MPU6050_SELF_TEST_A = 0x10,
  MPU6050_SMPLRT_DIV = 0x19,
  MPU6050_CONFIG = 0x1A,
  MPU6050_GYRO_CONFIG = 0x1B,
  MPU6050_ACCEL_CONFIG = 0x1C,
  MPU6050_FIFO_EN = 0x23,
  MPU6050_I2C_MST_CTRL = 0x24,
  MPU6050_I2C_SLV0_ADDR = 0x25,
  MPU6050_I2C_SLV0_REG = 0x26,
  MPU6050_I2C_SLV0_CTRL = 0x27,
  MPU6050_I2C_SLV1_ADDR = 0x28,
  MPU6050_I2C_SLV1_REG = 0x29,
  MPU6050_I2C_SLV1_CTRL = 0x2A,
  MPU6050_I2C_SLV2_ADDR = 0x2B,
  MPU6050_I2C_SLV2_REG = 0x2C,
  MPU6050_I2C_SLV2_CTRL = 0x2D,
  MPU6050_I2C_SLV3_ADDR = 0x2E,
  MPU6050_I2C_SLV3_REG = 0x2F,
  MPU6050_I2C_SLV3_CTRL = 0x30,
  MPU6050_I2C_SLV4_ADDR = 0x31,
  MPU6050_I2C_SLV4_REG = 0x32,
  MPU6050_I2C_SLV4_DO = 0x33,
  MPU6050_I2C_SLV4_CTRL = 0x34,
  MPU6050_I2C_SLV4_DI = 0x35,
  MPU6050_I2C_MST_STATUS = 0x36,
  MPU6050_INT_PIN_CFG = 0x37,
  MPU6050_INT_ENABLE = 0x38,
  MPU6050_DMP_INT_STATUS = 0x39,
  MPU6050_INT_STATUS = 0x3A,
  MPU6050_ACCEL_XOUT_H = 0x3B,
  MPU6050_ACCEL_XOUT_L = 0x3C,
  MPU6050_ACCEL_YOUT_H = 0x3D,
  MPU6050_ACCEL_YOUT_L = 0x3E,
  MPU6050_ACCEL_ZOUT_H = 0x3F,
  MPU6050_ACCEL_ZOUT_L = 0x40,
  MPU6050_TEMP_OUT_H = 0x41,
  MPU6050_TEMP_OUT_L = 0x42,
  MPU6050_GYRO_XOUT_H = 0x43,
  MPU6050_GYRO_XOUT_L = 0x44,
  MPU6050_GYRO_YOUT_H = 0x45,
  MPU6050_GYRO_YOUT_L = 0x46,
  MPU6050_GYRO_ZOUT_H = 0x47,
  MPU6050_GYRO_ZOUT_L = 0x48,
  MPU6050_EXT_SENS_DATA_00 = 0x49,
  MPU6050_EXT_SENS_DATA_01 = 0x4A,
  MPU6050_EXT_SENS_DATA_02 = 0x4B,
  MPU6050_EXT_SENS_DATA_03 = 0x4C,
  MPU6050_EXT_SENS_DATA_04 = 0x4D,
  MPU6050_EXT_SENS_DATA_05 = 0x4E,
  MPU6050_EXT_SENS_DATA_06 = 0x4F,
  MPU6050_EXT_SENS_DATA_07 = 0x50,
  MPU6050_EXT_SENS_DATA_08 = 0x51,
  MPU6050_EXT_SENS_DATA_09 = 0x52,
  MPU6050_EXT_SENS_DATA_10 = 0x53,
  MPU6050_EXT_SENS_DATA_11 = 0x54,
  MPU6050_EXT_SENS_DATA_12 = 0x55,
  MPU6050_EXT_SENS_DATA_13 = 0x56,
  MPU6050_EXT_SENS_DATA_14 = 0x57,
  MPU6050_EXT_SENS_DATA_15 = 0x58,
  MPU6050_EXT_SENS_DATA_16 = 0x59,
  MPU6050_EXT_SENS_DATA_17 = 0x5A,
  MPU6050_EXT_SENS_DATA_18 = 0x5B,
  MPU6050_EXT_SENS_DATA_19 = 0x5C,
  MPU6050_EXT_SENS_DATA_20 = 0x5D,
  MPU6050_EXT_SENS_DATA_21 = 0x5E,
  MPU6050_EXT_SENS_DATA_22 = 0x5F,
  MPU6050_EXT_SENS_DATA_23 = 0x60,
  MPU6050_I2C_SLV0_DO = 0x63,
  MPU6050_I2C_SLV1_DO = 0x64,
  MPU6050_I2C_SLV2_DO = 0x65,
  MPU6050_I2C_SLV3_DO = 0x66,
  MPU6050_I2C_MST_DELAY_CTRL = 0x67,
  MPU6050_SIGNAL_PATH_RESET = 0x68,
  MPU6050_USER_CTRL = 0x6A,
  MPU6050_PWR_MGMT_1 = 0x6B,
  MPU6050_PWR_MGMT_2 = 0x6C,
  MPU6050_FIFO_COUNTH = 0x72,
  MPU6050_FIFO_COUNTL = 0x73,
  MPU6050_FIFO_R_W = 0x74,
  MPU6050_WHO_AM_I = 0x75
};

/**
 * @brief FSYNC output values
 *
 * Allowed values for `setFsyncSampleOutput`.
 */
typedef enum fsync_out {
  MPU6050_FSYNC_OUT_DISABLED,
  MPU6050_FSYNC_OUT_TEMP,
  MPU6050_FSYNC_OUT_GYROX,
  MPU6050_FSYNC_OUT_GYROY,
  MPU6050_FSYNC_OUT_GYROZ,
  MPU6050_FSYNC_OUT_ACCELX,
  MPU6050_FSYNC_OUT_ACCELY,
  MPU6050_FSYNC_OUT_ACCEL_Z,
} mpu6050_fsync_out_t;

/**
 * @brief Clock source options
 *
 * Allowed values for `setClock`.
 */
typedef enum clock_select {
  MPU6050_INTR_8MHz,
  MPU6050_PLL_GYROX,
  MPU6050_PLL_GYROY,
  MPU6050_PLL_GYROZ,
  MPU6050_PLL_EXT_32K,
  MPU6050_PLL_EXT_19MHz,
  MPU6050_STOP = 7,
} mpu6050_clock_select_t;

/**
 * @brief Accelerometer range options
 *
 * Allowed values for `setAccelerometerRange`.
 */
typedef enum {
  MPU6050_RANGE_2_G = 0,  ///< +/- 2g (default value)
  MPU6050_RANGE_4_G = 1,  ///< +/- 4g
  MPU6050_RANGE_8_G = 2,  ///< +/- 8g
  MPU6050_RANGE_16_G = 3, ///< +/- 16g
} mpu6050_accel_range_t;

/**
 * @brief Gyroscope range options
 *
 * Allowed values for `setGyroRange`.
 */
typedef enum {
  MPU6050_RANGE_250_DEG,  ///< +/- 250 deg/s (default value)
  MPU6050_RANGE_500_DEG,  ///< +/- 500 deg/s
  MPU6050_RANGE_1000_DEG, ///< +/- 1000 deg/s
  MPU6050_RANGE_2000_DEG, ///< +/- 2000 deg/s
} mpu6050_gyro_range_t;

/**
 * @brief Digital low pass filter bandthwidth options
 *
 * Allowed values for `setFilterBandwidth`.
 */
typedef enum {
  MPU6050_BAND_260_HZ, ///< Docs imply this disables the filter
  MPU6050_BAND_184_HZ, ///< 184 Hz
  MPU6050_BAND_94_HZ,  ///< 94 Hz
  MPU6050_BAND_44_HZ,  ///< 44 Hz
  MPU6050_BAND_21_HZ,  ///< 21 Hz
  MPU6050_BAND_10_HZ,  ///< 10 Hz
  MPU6050_BAND_5_HZ,   ///< 5 Hz
} mpu6050_bandwidth_t;

/**
 * @brief Accelerometer high pass filter options
 *
 * Allowed values for `setHighPassFilter`.
 */
typedef enum {
  MPU6050_HIGHPASS_DISABLE,
  MPU6050_HIGHPASS_5_HZ,
  MPU6050_HIGHPASS_2_5_HZ,
  MPU6050_HIGHPASS_1_25_HZ,
  MPU6050_HIGHPASS_0_63_HZ,
  MPU6050_HIGHPASS_UNUSED,
  MPU6050_HIGHPASS_HOLD,
} mpu6050_highpass_t;

/**
 * @brief Periodic measurement options
 *
 * Allowed values for `setCycleRate`.
 */
typedef enum {
  MPU6050_CYCLE_1_25_HZ, ///< 1.25 Hz
  MPU6050_CYCLE_5_HZ,    ///< 5 Hz
  MPU6050_CYCLE_20_HZ,   ///< 20 Hz
  MPU6050_CYCLE_40_HZ,   ///< 40 Hz
} mpu6050_cycle_rate_t;

typedef struct {
  int16_t raw_acce_x;
  int16_t raw_acce_y;
  int16_t raw_acce_z;
} mpu6050_raw_acce_value_t;

typedef struct {
  int16_t raw_gyro_x;
  int16_t raw_gyro_y;
  int16_t raw_gyro_z;
} mpu6050_raw_gyro_value_t;

typedef struct {
  float acce_x;
  float acce_y;
  float acce_z;
} mpu6050_acce_value_t;

typedef struct {
  float gyro_x;
  float gyro_y;
  float gyro_z;
} mpu6050_gyro_value_t;

typedef struct {
  float roll;
  float pitch;
} complimentary_angle_t;

int mpu6050_begin(uint8_t slave);
void mpu6050_get_raw_gyro(mpu6050_raw_gyro_value_t *raw_gyro_value);
void mpu6050_get_raw_acce(mpu6050_raw_acce_value_t *raw_acce_value);
float mpu6050_get_acce_sensitivity(void);
float mpu6050_get_gyro_sensitivity(void);
int mpu6050_get_gyro(mpu6050_gyro_value_t *gyro_value);
int mpu6050_get_acce(mpu6050_acce_value_t *acce_value);
int mpu6050_set_gyro_fs(mpu6050_gyro_range_t gyro_fs);
int mpu6050_set_acce_fs(mpu6050_accel_range_t acce_fs);
int mpu6050_wake_up(void);

#ifdef __cplusplus
}
#endif
#endif // MPU6050_H
