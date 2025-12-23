/*
 * nes-cartridge-debugger-v2
 * 
 * Copyright (c) 2025 Dr. Joshua Butler
 * All rights reserved.

 * command.c
 *
 *  Created on: 16 Dec 2025
 *      Author: josh
 */

#include "command.h"
#include "ui.h"
#include <ctype.h>

/*-----------------------------------------------------------------------------
 * List of valid console/PC commands
 *---------------------------------------------------------------------------*/

typedef struct {
	command_t cmd;
	const char *cmd_str;
	uint8_t n_params;
} parse_args_t;

// @formatter:off
// Parsing information for each command {ID, command_string, num_parameters}
const parse_args_t valid_commands[] = {
	{INVALID_COMMAND, "", 0},
	{INVALID_PARAMETER_COUNT, "", 0},
	{CMD_HELP, "help", 0},
	{CMD_READ_PRG_ROM, "read_prg_rom", 2},
	{CMD_READ_CHR_ROM, "read_chr_rom", 2},
	{CMD_TEST_OUTPUT_BUS, "test_output_bus", 0},
	{CMD_TEST_INPUT_BUS, "test_input_bus", 0},
	{CMD_SET_MAPPER, "set_mapper", 1},
	{CMD_GET_MAPPER, "get_mapper", 0},
	{CMD_GET_AVAIL_MAPPERS, "get_avail_mappers", 0},
	{CMD_SWITCH_PRG_BANK, "switch_prg_bank", 1},
	{CMD_SWITCH_CHR_BANK, "switch_chr_bank", 1},
	{CMD_EXIT, "exit", 0},
	{NUM_COMMANDS, NULL, 0}
};
// @formatter:on


/**
 * @brief Count the number of parameters in a string
 * @param char *parameter: Parameter string
 * @return uint8_t: Number of parameters
 */
uint8_t num_parameters(char *parameters) {
    uint8_t count = 1;
    uint8_t str_len = strlen(parameters);
    if (str_len == 0) {
        return 0;
    }

    for (int i = 0; i < str_len; i++) {
        if (isspace((unsigned char )parameters[i])) {
            count++;
        }
    }
    return count;
}

/**
 * @brief Trim whitespace from a string
 * @param char *dst: Destination string
 * @param char *src: Source string
 * @return None
 */
void trim_whitespace(char *dst, const char *src) {
    uint8_t i = 0;
    uint8_t j = strlen(src);
    while (isspace((unsigned char )src[i])) {
        i++;
    }
    while (isspace((unsigned char )src[j])) {
        j--;
    }
    strncpy(dst, src + i, j - i + 1);
    dst[j] = '\0';
}

/**
 * @brief Reentrant version of strtok
 * @param char *str: String to parse
 * @param const char *delim: Delimiter
 * @param char **saveptr: Pointer to save the next token
 * @return char*: Token
 */
char* strtok_r(char *str, const char *delim, char **saveptr) {
    char *end;

    // If str is NULL, continue with the previous string.
    if (str == NULL) {
        str = *saveptr;
    }

    // Skip leading delimiters.
    str += strspn(str, delim);
    if (*str == '\0') {
        // No more tokens.
        *saveptr = str;
        return NULL;
    }

    // Find the end of the token.
    end = str + strcspn(str, delim);
    if (*end == '\0') {
        // This is the last token.
        *saveptr = end;
    } else {
        // Terminate the token and save the rest for the next call.
        *end = '\0';
        *saveptr = end + 1;
    }

    return str;
}

char* strtok_safe(char *str, const char *delim, char **saveptr) {
    return strtok_r(str, delim, saveptr);
}

/**
 * @brief Parse a command string
 * @param uint8_t *command: Command string
 * @param uint8_t *token: Token to return
 * @param uint8_t *parameter: Parameter to return
 * @return command_t: Command token
 */
command_t parse_command(uint8_t *command, uint8_t *token, uint8_t *parameter, uint16_t token_len,
        uint16_t param_len) {

    char tmp_parameter[64];
    char *tmp_param_ptr;
    char *tmp_ptr;

    memset(tmp_parameter, 0, sizeof(tmp_parameter));

    strncpy((char*) token, strtok_safe((char*) command, " ", &tmp_param_ptr), token_len);
    tmp_ptr = strtok_safe(NULL, "", &tmp_param_ptr);
    if (tmp_ptr != NULL) {
        strncpy((char*) parameter, tmp_ptr, param_len);
    } else {
        strncpy((char*) parameter, "", param_len);
    }

    trim_whitespace(tmp_parameter, (char*) parameter);
    strncpy((char*) parameter, tmp_parameter, param_len);

    uint8_t num_params = num_parameters((char*) parameter);

    for (uint8_t i = 0; i < NUM_COMMANDS; i++) {
        if (strcmp((char*) token, valid_commands[i].cmd_str) == 0) {
            if (valid_commands[i].n_params != num_params) {
                return CMD_INVALID_PARAMETER_COUNT;
            }
            return valid_commands[i].cmd;
        }
    }

    return INVALID_COMMAND;
}

/**
 * @brief Parse a parameter string
 * @param parameter - parameter string
 * @param parsed_parameter - parsed parameter
 * @param max_size - maximum size of the parsed parameter
 * @param index - index of the parameter to return
 * @return 1 if successful, 0 if failed
 */
uint8_t parse_parameter(uint8_t *parameter, uint8_t *parsed_parameter, uint8_t max_size, uint8_t index) {
    uint8_t *p = 0;
    uint8_t j = 0;

    memset(parsed_parameter, 0, max_size);
    p = parameter;
    if (index) {
        for (uint8_t i = 0; i < strlen((char*) parameter); i++) {
            if (isspace((unsigned char ) parameter[i])) {
                index--;
            }
            p++;
            if (index == 0) {
                break;
            }
        }
    }

    if (index > 0) {
        return 0;
    }
    while (!isspace((unsigned char ) *p) && *p != '\0') {
        parsed_parameter[j++] = *p;
        p++;
    }
    parsed_parameter[j] = '\0';
    return 1;
}
