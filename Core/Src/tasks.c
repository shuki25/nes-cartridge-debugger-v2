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

#define BLOCK_SIZE (256)

static uint8_t rom_data_block[BLOCK_SIZE] = { 0 };
static char output_buffer[128] = { 0 };

// External Variables
extern uint8_t mapper_count;
extern mapper_t mapper_NES[];

command_status_t task_read_prg_rom(app_t *app, uint32_t start_address, uint16_t num_blocks) {

    uint32_t read_size = num_blocks * BLOCK_SIZE; // PRG ROM size in bytes
    uint32_t address = 0;
    uint32_t start_time = 0;
    uint32_t end_time = 0;
    uint32_t elapsed_time = 0;
    uint32_t avg_elapsed_time = 0;
    uint16_t num_iterations = 0;
    uint32_t bytes_per_second = 0;
    uint16_t block = 0;
    uint32_t total_data_read = 0;

    PRINT_FMT_DEBUG("Reading PRG ROM of size: %ld bytes\n", read_size);

    while (num_blocks > block) {

        snprintf(output_buffer, sizeof(output_buffer), "OK\tblock: %d\tsize: %d\n", block, BLOCK_SIZE);
        print_terminal(output_buffer);

        address = start_address + (block * BLOCK_SIZE);
        start_time = TIM2->CNT;

        data_bus_read_prg_block(&app->data_bus, address, rom_data_block, 256);
        // Print the data to the debug console
//            print_hex(rom_data_block, 256, 16, address);
        print_raw_data(rom_data_block, BLOCK_SIZE);
        end_time = TIM2->CNT;
        elapsed_time += time_diff_rollover(start_time, end_time, 0xFFFFFFFF);
        block++;
        num_iterations++;
        total_data_read += BLOCK_SIZE;
    }

    print_terminal("DONE\tFinished reading PRG ROM\n");
    avg_elapsed_time = elapsed_time / num_iterations;
    bytes_per_second = calculate_data_rate(read_size, avg_elapsed_time);

    PRINT_DEBUG("Finished reading PRG ROM\n");

    PRINT_FMT_DEBUG("Elapsed Time: %ld us, %ld ms\n", elapsed_time, (elapsed_time / 1000));
    PRINT_FMT_DEBUG("Data Rate: %ld Bps, %ld KBps\n", bytes_per_second, (bytes_per_second / 1024));
    return CMD_OK;
}

void task_get_mapper_name(app_t *app) {
    char *mapper_name;
    mapper_name = mapper_get_name(app->cart.mapper.mapper_id);
    snprintf(output_buffer, sizeof(output_buffer), "OK\tMapper ID: %d\tName: %s\n",
            app->cart.mapper.mapper_id, mapper_name);
    print_terminal(output_buffer);
}

void task_list_available_mappers() {
    print_terminal("OK\tAvailable Mappers:\n");
    for (uint8_t i = 0; i < mapper_count; i++) {
        snprintf(output_buffer, sizeof(output_buffer), "%d\t%s\n", mapper_NES[i].mapper_id,
                mapper_get_name(mapper_NES[i].mapper_id));
        print_terminal(output_buffer);
    }
    print_terminal("DONE\tEnd of available mappers\n");
}

void task_execute_command(app_t *app) {
    command_status_t cmd_status = CMD_OK;
    uint8_t parameter_buffer[PARAM_LEN];
    memset(parameter_buffer, 0, PARAM_LEN);
    uint32_t start_address = 0;
    uint16_t num_blocks = 0;

    switch (app->parsed_command) {
    case CMD_HELP:
        print_help();
        break;
    case CMD_READ_PRG_ROM:
        parse_parameter(app->parameter, parameter_buffer, PARAM_LEN, 0);
        start_address = (uint32_t) strtoul((char*) parameter_buffer, NULL, 16);
        parse_parameter(app->parameter, parameter_buffer, PARAM_LEN, 1);
        num_blocks = (uint16_t) atoi((char*) parameter_buffer);
        cmd_status = task_read_prg_rom(app, start_address, num_blocks);
        break;
    case CMD_SET_MAPPER:
        parse_parameter(app->parameter, parameter_buffer, PARAM_LEN, 0);
        mapper_status_t mapper_status;
        uint8_t mapper_id = (uint8_t) atoi((char*) parameter_buffer);
        mapper_status = mapper_init(&app->cart.mapper, mapper_id);
        if (mapper_status == MAPPER_OK) {
            snprintf(output_buffer, sizeof(output_buffer), "OK\tMapper set to ID: %d\tName: %s\n", mapper_id,
                    mapper_get_name(mapper_id));
            print_terminal(output_buffer);
        } else if (mapper_status == MAPPER_NOT_FOUND) {
            snprintf(output_buffer, sizeof(output_buffer), "ERR\tMapper ID: %d not found\n", mapper_id);
            print_terminal(output_buffer);
        }
        break;
    case CMD_GET_MAPPER:
        task_get_mapper_name(app);
        break;
    case CMD_GET_AVAIL_MAPPERS:
        task_list_available_mappers();
        break;
    case CMD_SWITCH_PRG_BANK:
        parse_parameter(app->parameter, parameter_buffer, PARAM_LEN, 0);
        cartridge_status_t cartridge_status;
        uint8_t bank = (uint8_t) atoi((char*) parameter_buffer);
        cartridge_status = cartridge_switch_prg_bank(&app->cart, bank);
        if (cartridge_status == CART_OK) {
            snprintf(output_buffer, sizeof(output_buffer), "OK\tSwitched to PRG bank: %d\n", bank);
            print_terminal(output_buffer);
        } else {
            snprintf(output_buffer, sizeof(output_buffer), "ERR\tFailed to switch to PRG bank: %d\n", bank);
            print_terminal(output_buffer);
        }
        break;
    default:
        PRINT_FMT_DEBUG("Unknown command ID: %d\r\n", app->parsed_command);
    }

    if (cmd_status != CMD_OK) {
        PRINT_FMT_DEBUG("Command execution failed with status: %d\n", cmd_status);
    }

    app->state_machine.current_state = STATE_WAIT_FOR_COMMAND;
}
