/*
 * commands.c
 *
 *  Created on: Apr 28, 2024
 *      Author: josh
 */

#include "commands.h"
#include "ui.h"
#include <ctype.h>

/*-----------------------------------------------------------------------------
 * List of valid console/PC commands
 *---------------------------------------------------------------------------*/
// Must align with command_t
const char *valid_commands[] = { "", "", "help", "bat_mgr", "scan_i2c", "psensor", "bat_gauge",
        "flash_memory", "serial_number", "reset", "set_date", "set_time", "date_time", "test_led",
        "get_wiper", "set_wiper", "read_psensor", "flash_mem_status", "flash_mem_test", "flash_mem_erase",
        NULL };
uint8_t valid_num_params[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0 };

/*-----------------------------------------------------------------------------
 * Function: trim_whitespace
 *
 * This function will trim whitespace from a string
 *
 * Parameters: char *dst - destination string
 *            char *src - source string
 * Return: None
 *
 *--------------------------------------------------------------------------- */
void trim_whitespace(char *dst, char *src) {
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

/*-----------------------------------------------------------------------------
 * Function: num_parameters
 *
 * This function will count the number of parameters in a string
 *
 * Parameters: char *parameters - string of parameters
 * Return: uint8_t - number of parameters
 *---------------------------------------------------------------------------*/
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

/*-----------------------------------------------------------------------------
 * Function: parse_command
 *
 * This function will parse a command string and return the command token
 *
 * Parameters: uint8_t *command - command string
 *             uint8_t *token - token to return
 *             uint8_t *parameter - parameter to return
 * Return: command_t - command token
 *---------------------------------------------------------------------------*/
command_t parse_command(uint8_t *command, uint8_t *token, uint8_t *parameter) {

    char tmp_parameter[64];
    char *tmp_param_ptr;
    char *tmp_ptr;

    memset(tmp_parameter, 0, sizeof(tmp_parameter));

    strcpy((char*) token, strtok_safe((char*) command, " ", &tmp_param_ptr));
    tmp_ptr = strtok_safe(NULL, "", &tmp_param_ptr);
    if (tmp_ptr != NULL) {
        strcpy((char*) parameter, tmp_ptr);
    } else {
        strcpy((char*) parameter, "");
    }
//    strcpy((char*) parameter, strtok_safe(NULL, "", &tmp_param_ptr));
//    sscanf((char*) command, "%s %s", token, (char*) parameter);
    uint8_t i = 0;

    trim_whitespace(tmp_parameter, (char*) parameter);
    strcpy((char*) parameter, tmp_parameter);

    uint8_t num_params = num_parameters((char*) parameter);

    while (valid_commands[i] != NULL) {
        if (strcmp((char*) token, valid_commands[i]) == 0) {
            if (valid_num_params[i] != num_params) {
                return INVALID_PARAMETER_COUNT;
            }
            return i;
        }
        i++;
    }
    return INVALID_COMMAND;
}

/*-----------------------------------------------------------------------------
 * Function: parse_date
 *
 * This function will parse a date string and return the year, month, and day
 *
 * Parameters: uint8_t *date - date string
 *             uint16_t *year - year to return
 *             uint16_t *month - month to return
 *             uint16_t *day - day to return
 * Return: uint8_t - 1 if successful, 0 if failed
 *---------------------------------------------------------------------------*/
uint8_t parse_date(uint8_t *date, uint16_t *year, uint16_t *month, uint16_t *day) {
    uint8_t num_returned;
    int y, m, d;

    num_returned = sscanf((char*) date, "%d-%d-%d", &y, &m, &d);
    if (num_returned < 3) {
        return 0;
    } else {
        *year = y;
        *month = m;
        *day = d;
        return 1;
    }
}

/*-----------------------------------------------------------------------------
 * Function: parse_time
 *
 * This function will parse a time string and return the hour, minute, and second
 *
 * Parameters: uint8_t *time - time string
 *             uint16_t *hour - hour to return
 *             uint16_t *minute - minute to return
 *             uint16_t *second - second to return
 * Return: uint8_t - 1 if successful, 0 if failed
 *---------------------------------------------------------------------------*/
uint8_t parse_time(uint8_t *time, uint16_t *hour, uint16_t *minute, uint16_t *second, uint16_t *millisecond) {
    uint8_t num_returned;
    int h, m, s, ms;

    num_returned = sscanf((char*) time, "%d:%d:%d.%d", &h, &m, &s, &ms);
    if (num_returned < 4) {
        return 0;
    } else {
        *hour = h;
        *minute = m;
        *second = s;
        *millisecond = ms;
        return 1;
    }
}

/*-----------------------------------------------------------------------------
 * Function: execute_command
 *
 * This function will execute a command
 *
 * Parameters: command_t command - command token
 *            uint8_t *parameter - parameter string
 * Return: cmd_status_t - status of the command
 *---------------------------------------------------------------------------*/
cmd_status_t execute_command(command_t command, uint8_t *parameter) {

    char output_buffer[256];
    memset(output_buffer, 0, sizeof(output_buffer));

    switch (command) {
    default:
        sprintf(output_buffer, "\r\nInvalid command\n");
        print_terminal(output_buffer);
        return INVALID_COMMAND;
        break;
    }
    return CMD_OK;
}
