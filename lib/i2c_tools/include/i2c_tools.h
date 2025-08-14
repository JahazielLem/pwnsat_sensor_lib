/* include - i2c_tools.h
 * DESCRIPTION
 *
 * bme280 - LoRa Packet Analyzer
 * By astrobyte 13/08/25.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */
#ifndef I2C_TOOLS_H
#define I2C_TOOLS_H
#ifdef __cplusplus
extern "C" {
#endif

#include "bcm2835.h"
#include <stdint.h>

int i2c_tools_init(void);
int i2c_tools_set_slave_address(const uint8_t slave_addr);
void i2c_tools_set_baudrate(const uint32_t baudrate);
int i2c_tools_read_reg(const uint8_t reg_address, char *buffer, uint8_t length);
int i2c_tool_write_reg(const uint8_t reg_address, const uint8_t data);
uint8_t i2c_tool_read_byte(const uint8_t reg_address);
uint16_t i2c_tool_read16(const uint8_t reg_address);
int16_t i2c_tool_reads16(const uint8_t reg_address);
uint16_t i2c_tool_read16_le(const uint8_t reg_address);
int16_t i2c_tool_reads16_le(const uint8_t reg_address);
uint32_t i2c_tool_read24(const uint8_t reg_address);
int32_t i2c_tool_reads24(const uint8_t reg_address);
void i2c_tool_cleanup(void);
#ifdef __cplusplus
}
#endif
#endif // I2C_TOOLS_H
