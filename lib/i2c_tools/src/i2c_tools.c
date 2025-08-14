/* src - i2c_tools.c
 * DESCRIPTION
 *
 * bme280 - LoRa Packet Analyzer
 * By astrobyte 13/08/25.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */
#include <i2c_tools.h>
#include <stdint.h>

static uint8_t slave_address = 0x00;
static uint8_t initialized = 0;
static int ret = 0;

int i2c_tools_init(void) {
  ret = bcm2835_init();
  if (ret < 0) {
    return ret;
  }
  return BCM2835_I2C_REASON_OK;
}

int i2c_tools_set_slave_address(const uint8_t slave_addr) {
  ret = bcm2835_i2c_begin();
  if (ret < 0) {
    bcm2835_close();
    return ret;
  }
  slave_address = slave_addr;
  bcm2835_i2c_setSlaveAddress(slave_address);
  return BCM2835_I2C_REASON_OK;
}

void i2c_tools_set_baudrate(const uint32_t baudrate) {
  bcm2835_i2c_set_baudrate(baudrate);
}

int i2c_tools_read_reg(const uint8_t reg_address, char *buffer,
                       uint8_t length) {
  const char reg = (char)reg_address;
  uint8_t result = bcm2835_i2c_write(&reg, 1);
  if (result != BCM2835_I2C_REASON_OK) {
    return -3;
  }
  result = bcm2835_i2c_read(buffer, length);
  if (result != BCM2835_I2C_REASON_OK) {
    return -3;
  }
  return 0;
}

int i2c_tool_write_reg(const uint8_t reg_address, const uint8_t data) {
  const char buffer[2] = {(char)reg_address, (char)data};
  return bcm2835_i2c_write(buffer, 2);
}

uint8_t i2c_tool_read_byte(const uint8_t reg_address) {
  char buffer[1];
  i2c_tools_read_reg(reg_address, buffer, 1);
  return (uint8_t)buffer[0];
}

uint16_t i2c_tool_read16(const uint8_t reg_address) {
  char buffer[2];
  i2c_tools_read_reg(reg_address, buffer, 2);
  return ((uint16_t)(uint8_t)buffer[0]) << 8 | ((uint16_t)(uint8_t)buffer[1]);
}

int16_t i2c_tool_reads16(const uint8_t reg_address) {
  return (int16_t)i2c_tool_read16(reg_address);
}

uint16_t i2c_tool_read16_le(const uint8_t reg_address) {
  uint16_t temp = i2c_tool_read16(reg_address);
  return (temp >> 8) | (temp << 8);
}

int16_t i2c_tool_reads16_le(const uint8_t reg_address) {
  return (int16_t)i2c_tool_read16_le(reg_address);
}

uint32_t i2c_tool_read24(const uint8_t reg_address) {
  char buffer[3];
  i2c_tools_read_reg(reg_address, buffer, 3);
  return ((uint32_t)(uint8_t)buffer[0]) << 16 |
         ((uint32_t)(uint8_t)buffer[1]) << 8 | ((uint32_t)(uint8_t)buffer[2]);
}

int32_t i2c_tool_reads24(const uint8_t reg_address) {
  char buffer[3];
  i2c_tools_read_reg(reg_address, buffer, 3);
  return (int32_t)i2c_tool_read24(reg_address);
}

void i2c_tool_cleanup() {
  bcm2835_i2c_end();
  bcm2835_close();
}