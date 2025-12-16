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


#endif /* INC_COMMAND_H_ */
