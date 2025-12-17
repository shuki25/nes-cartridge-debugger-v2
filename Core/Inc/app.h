/*
 * nes-cartridge-debugger-v2
 * 
 * Copyright (c) 2025 Dr. Joshua Butler
 * All rights reserved.

 * app.h
 *
 *  Created on: 15 Dec 2025
 *      Author: josh
 */

#ifndef INC_APP_H_
#define INC_APP_H_
//
#include <stdint.h>
#include "led_indicator.h"
#include "data_bus.h"
#include "mapper.h"
#include "cartridge.h"
#include "command.h"

#define CMD_BUF_LEN (63)
#define CMD_LEN (31)
#define PARAM_LEN (31)
#define TX_BUF_LEN (127)

//@formatter:off
typedef enum {
	STATE_IDLE = 0,
	STATE_ERROR,
	STATE_INITIALIZE,
	STATE_SPLASH_SCREEN,
	STATE_TEST_OUTPUT_BUS,
	STATE_TEST_INPUT_BUS,
	STATE_READ_PRG_ROM,
	STATE_READ_CHR_ROM,
	STATE_PARSE_COMMAND,
	STATE_WAIT_FOR_COMMAND,
	STATE_EXECUTE_COMMAND
} state_t;
//@formatter:on

typedef struct {
	state_t current_state;
} state_machine_t;

typedef struct {
	state_machine_t state_machine;
	led_t status_led;
	led_t nes_clk_led;
	data_bus_config_t data_bus;
	cartridge_t cart;
	uint8_t data_bus_position;
	uint8_t test_phase;
	uint8_t prev_test_phase;
	uint8_t detected_change;
	uint16_t rom_address;
	uint16_t ppu_address;
	uint8_t rom_dump_flag;
	uint32_t start_time;
	uint32_t end_time;
	uint32_t elapsed_time;
	uint32_t bytes_per_second;
	char *mapper_name;
	command_t parsed_command;
	char parameter[PARAM_LEN + 1];
} app_t;

typedef struct {
    uint32_t start_address;
    uint32_t end_address;
    uint32_t next_address;
    uint16_t block_size;
} binary_download_t;

// Function prototypes

void app_init();
void app_loop(app_t *app);

#endif /* INC_APP_H_ */
