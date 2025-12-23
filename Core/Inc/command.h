/*
 * nes-cartridge-debugger-v2
 * 
 * Copyright (c) 2025 Dr. Joshua Butler
 * All rights reserved.

 * command.h
 *
 *  Created on: 16 Dec 2025
 *      Author: josh
 */

#ifndef INC_COMMAND_H_
#define INC_COMMAND_H_

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef enum command {
	INVALID_COMMAND,
	INVALID_PARAMETER_COUNT,
	CMD_HELP,
	CMD_READ_PRG_ROM,
	CMD_READ_CHR_ROM,
	CMD_TEST_OUTPUT_BUS,
	CMD_TEST_INPUT_BUS,
	CMD_SET_MAPPER,
	CMD_GET_MAPPER,
	CMD_GET_AVAIL_MAPPERS,
	CMD_SWITCH_PRG_BANK,
	CMD_SWITCH_CHR_BANK,
	CMD_EXIT,
	NUM_COMMANDS
} command_t;

typedef enum {
	CMD_OK = 0,
	CMD_ERROR,
	CMD_INVALID_PARAMETER_COUNT,
	CMD_EXECUTION_FAILED,
	CMD_UNKNOWN
} command_status_t;

// Function prototypes
uint8_t num_parameters(char *parameters);
void trim_whitespace(char *dst, const char *src);
char* strtok_safe(char *str, const char *delim, char **saveptr);
char* strtok_r(char *str, const char *delim, char **saveptr);
command_t parse_command(uint8_t *command, uint8_t *token,
		uint8_t *parameter, uint16_t token_len, uint16_t param_len);
uint8_t parse_parameter(uint8_t *parameter, uint8_t *parsed_parameter,
		uint8_t max_size, uint8_t index);

#endif /* INC_COMMAND_H_ */
