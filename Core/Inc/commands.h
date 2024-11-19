/*
 * commands.h
 *
 *  Created on: Apr 28, 2024
 *      Author: josh
 */

#ifndef INC_COMMANDS_H_
#define INC_COMMANDS_H_

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef enum command {
    INVALID_COMMAND,
    INVALID_PARAMETER_COUNT,
    CMD_HELP,
    CMD_BATTERY_MANAGEMENT,
    CMD_SCAN_I2C_BUS,
    CMD_PRESSURE_SENSOR,
    CMD_BATTERY_FUEL_GAUGE,
    CMD_FLASH_MEMORY,
    CMD_SERIAL_NUMBER,
    CMD_RESET,
    CMD_SET_DATE,
    CMD_SET_TIME,
    CMD_DATE_TIME,
    CMD_TEST_LED,
    CMD_GET_WIPER,
    CMD_SET_WIPER,
    CMD_READ_PRESSURE_SENSOR,
    CMD_GET_FLASH_MEM_STATE,
    CMD_RUN_FLASH_MEM_TEST,
    CMD_ERASE_FLASH_MEM,
    NUM_COMMANDS
} command_t;

typedef enum cmd_status {
    CMD_OK, CMD_ERROR, CMD_INVALID
} cmd_status_t;

command_t parse_command(uint8_t *command, uint8_t *token, uint8_t *parameter);
uint8_t parse_time(uint8_t *time, uint16_t *hour, uint16_t *minute, uint16_t *second, uint16_t *millisecond);
uint8_t parse_date(uint8_t *date, uint16_t *year, uint16_t *month, uint16_t *day);
cmd_status_t execute_command(command_t command, uint8_t *parameter);
uint32_t parse_i2c_command(command_t cmd_token, uint8_t *parameter);
#endif /* INC_COMMANDS_H_ */
