/**
 * @file main.c
 * @brief Main program for Pwnsat LoRa Packet Analyzer
 * @author Pwnsat Team
 * @date 2025-08-13
 * @license GNU General Public License v3.0
 */

#include "mpu6050.h"
#include <bme280.h>
#include <stdio.h>

int main() {
  if (bme280_begin(BME280_ADDRESS_ALTERNATE) != 0) {
    fprintf(stderr, "Error inicializando BME280\n");
    return -1;
  }

  if (mpu6050_begin(MPU6050_ADDRESS) != 0) {
    fprintf(stderr, "Error inicializando MPU6050\n");
    return -1;
  }

  mpu6050_set_acce_fs(MPU6050_RANGE_4_G);
  mpu6050_set_gyro_fs(MPU6050_RANGE_500_DEG);

  mpu6050_acce_value_t acce;
  mpu6050_gyro_value_t gyro;

  uint8_t counter = 0;
  while (counter < 30) {
    float temp = bme280_read_temperature();
    float pressure = bme280_read_pressure();
    float humidity = bme280_read_humidity();
    float altitude = bme280_read_altitude(SEALEVELPRESSURE_HPA);

    printf("[BME] Temp: %.2f Press: %.2f Hum: %.2f Alt: %.2f\n", temp, pressure,
           humidity, altitude);
    bcm2835_delay(100);
    mpu6050_get_acce(&acce);
    mpu6050_get_gyro(&gyro);
    printf("[MPU] AcceX: %.2f AcceY: %.2f AcceZ: %.2f GyroX: %.2f GyroY: %.2f "
           "GyroZ: %.2f\n",
           acce.acce_x, acce.acce_y, acce.acce_z, gyro.gyro_x, gyro.gyro_y,
           gyro.gyro_z);

    bcm2835_delay(2000);
    counter++;
  }

  return 0;
}