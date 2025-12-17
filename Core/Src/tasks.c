/*
 * nes-cartridge-debugger-v2
 * 
 * Copyright (c) 2025 Dr. Joshua Butler
 * All rights reserved.

 * tasks.c
 *
 *  Created on: 16 Dec 2025
 *      Author: josh
 */

#include "tasks.h"
#include "ui.h"
#include "util.h"
#include "debug.h"
#include "mapper.h"
#include "cartridge.h"
#include "data_bus.h"

static uint8_t rom_data_block[256] = { 0 };

command_status_t task_read_prg_rom(app_t *app) {
	uint32_t rom_size = 32 * 1024; // PRG ROM size in bytes
	uint32_t address = 0x8000;
	uint32_t start_time = 0;
	uint32_t end_time = 0;
	uint32_t elapsed_time = 0;
	uint32_t bytes_per_second = 0;

	PRINT_FMT_DEBUG("Reading PRG ROM of size: %d bytes\n", rom_size);

	start_time = TIM2->CNT;
	for (address = 0x8000; address < (0x8000 + rom_size); address += 256) {
		data_bus_read_prg_block(&app->data_bus, address, rom_data_block, 256);
		// Print the data to the debug console
		print_hex(rom_data_block, 256, 16, address);
	}
	end_time = TIM2->CNT;
	elapsed_time = end_time - start_time;
	bytes_per_second = calculate_data_rate(rom_size, elapsed_time);
	PRINT_DEBUG("Finished reading PRG ROM\n");

	PRINT_FMT_DEBUG("Elapsed Time: %ld us, %ld ms\n", elapsed_time,
			(elapsed_time / 1000));
	PRINT_FMT_DEBUG("Data Rate: %ld Bps, %ld KBps\n", bytes_per_second,
			(bytes_per_second / 1024));

	return CMD_OK;
}

void task_execute_command(app_t *app) {
	command_status_t cmd_status = CMD_OK;

	switch (app->parsed_command) {
	case CMD_HELP:
		print_help();
		break;
	case CMD_READ_PRG_ROM:
		cmd_status = task_read_prg_rom(app);
		break;
	default:
		PRINT_FMT_DEBUG("Unknown command ID: %d\r\n", app->parsed_command);
	}

	if (cmd_status != CMD_OK) {
		PRINT_FMT_DEBUG("Command execution failed with status: %d\n",
				cmd_status);
	}

	app->state_machine.current_state = STATE_WAIT_FOR_COMMAND;
}
