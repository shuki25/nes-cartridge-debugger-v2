/*
 * ui.c
 *
 *  Created on: Mar 14, 2024
 *      Author: josh
 */

#include "ui.h"
#include "ssd1306.h"

/*-----------------------------------------------------------------------------
 * Function: echo_terminal
 *
 * This function will echo a character to the terminal. Other modes will ignore.
 *
 * Parameters: scoreboard_t *s - pointer to the scoreboard
 *             uint8_t *rx_value - character to echo
 * Return: None
 *---------------------------------------------------------------------------*/
void echo_terminal(uint8_t *rx_value) {
    HAL_Delay(1);
    CDC_Transmit_FS(rx_value, 1);
}

/*-----------------------------------------------------------------------------
 * Function: print_terminal
 *
 * This function will print a message to the terminal. Other modes will ignore.
 *
 * Parameters: scoreboard_t *s - pointer to the scoreboard
 *             char *message - message to print
 * Return: None
 *---------------------------------------------------------------------------*/
void print_terminal(char *message) {
    HAL_Delay(1);
    CDC_Transmit_FS((uint8_t*) message, strlen(message));
}

/*-----------------------------------------------------------------------------
 * Function: print_divider
 *
 * This function will print a divider to the terminal.
 *
 * Parameters: uint8_t length - length of the divider
 * Return: None
 *---------------------------------------------------------------------------*/
void print_divider(uint8_t length) {
    char divider[UI_BUFFER_SIZE] = {0};
    memset(divider, '-', length);
    divider[length] = '\r';
    divider[length + 1] = '\n';
    divider[length + 2] = '\0';
    print_terminal(divider);
}

/*-----------------------------------------------------------------------------
 * Function: print_help
 *
 * This function will print the help menu to the terminal.
 *
 * Parameters: None
 * Return: None
 *---------------------------------------------------------------------------*/
void print_help() {
    print_divider(80);
    print_terminal("read_prg_rom <addr>: Read ROM data from PRG\n");
    print_terminal("read_chr_rom <addr>: Read ROM data from CHR\n");
    print_terminal("test_output_bus: Test the output data bus\n");
    print_terminal("test_input_bus: Test the input data bus\n");
    print_terminal("set_mapper <id>: Set the mapper ID\n");
    print_terminal("switch_prg_bank <bank>: Switch the PRG bank\n");
    print_terminal("switch_chr_bank <bank>: Switch the CHR bank\n");
    print_terminal("exit: Exit the tester\n");
    print_divider(80);
}

///*-----------------------------------------------------------------------------
// * Function: print_banner
// *
// * This function will print the banner to the terminal.
// *
// * Parameters: None
// * Return: None
// *---------------------------------------------------------------------------*/
//void print_banner() {
//    print_divider(80);
//    print_terminal("Welcome to the QSPI W25Q Flash Memory Tester\r\n");
//    print_divider(80);
//}

/*-----------------------------------------------------------------------------
 * Function: print_hex
 * This function will print a hex dump to the terminal.
 * Parameters: uint8_t *data - pointer to the data
 *             uint16_t len - length of the data
 *             uint8_t data_width - width of the data
 * Return: None
 *---------------------------------------------------------------------------*/
void print_hex(uint8_t *data, uint16_t len, uint8_t data_width, uint32_t start_addr) {
    if (len == 0 || data == NULL) {
        return;
    }

    const uint8_t BYTES_PER_LINE = 16;
    uint32_t offset = 0;
    uint32_t address = start_addr;
    char line[UI_BUFFER_SIZE];
    const char hex_tbl[] = "0123456789ABCDEF";

    /* Ensure data_width is sane */
    uint8_t target_width = data_width == 0 ? BYTES_PER_LINE : data_width;
    if (target_width > BYTES_PER_LINE) {
        target_width = BYTES_PER_LINE;
    }

    while (offset < len) {
        uint8_t bytes_this_line = (uint32_t)(len - offset) >= BYTES_PER_LINE ? BYTES_PER_LINE : (uint8_t)(len - offset);
        char *p = line;
        size_t rem = sizeof(line);

        /* Address prefix */
        int n = snprintf(p, rem, "OK\t0x%08lX:", (unsigned long)address);
        if (n < 0) return;
        p += n; rem -= (size_t)n;

        /* Build hex area. Keep grouping space every 4 bytes (including before first group) */
        for (uint8_t j = 0; j < BYTES_PER_LINE; j++) {
            /* group space */
            if ((j % 4) == 0) {
                if (rem > 1) { *p++ = ' '; rem--; }
            }

            if (j < bytes_this_line) {
                uint8_t b = data[offset + j];
                /* two hex chars */
                if (rem > 2) {
                    *p++ = hex_tbl[(b >> 4) & 0x0F]; rem--;
                    *p++ = hex_tbl[b & 0x0F]; rem--;
                }
                /* trailing space after byte */
                if (rem > 0) { *p++ = ' '; rem--; }
            } else {
                /* pad missing byte with three spaces to keep alignment */
                if (rem > 2) {
                    *p++ = ' '; *p++ = ' '; *p++ = ' '; rem -= 3;
                } else {
                    /* if not enough room, stop adding */
                    break;
                }
            }
        }

        /* spacer between hex area and ASCII */
        if (rem > 1) { *p++ = ' '; *p++ = ' '; rem -= 2; }

        /* ASCII representation for present bytes */
        for (uint8_t j = 0; j < bytes_this_line; j++) {
            char c = (char)data[offset + j];
            if (c < 32 || c > 126) c = '.';
            if (rem > 1) { *p++ = c; rem--; } else break;
        }

        /* CRLF */
        if (rem > 1) { *p++ = '\r'; *p++ = '\n'; rem -= 2; }
        /* ensure null termination */
        if (rem > 0) *p = '\0'; else line[sizeof(line)-1] = '\0';

        print_terminal(line);

        offset += bytes_this_line;
        address += bytes_this_line;
    }
}


/*-----------------------------------------------------------------------------
 * Function: ui_latency_ms
 *
 * This function will print the latency to the OLED screen.
 *
 * Parameters: uint32_t latency - latency in milliseconds
 * Return: None
 *---------------------------------------------------------------------------*/
void print_latency_ms(uint32_t latency) {
    uint8_t oled_buffer[16];

    snprintf((char*) oled_buffer, 16, " %3lu.%0lu ms", (uint32_t) latency / 1000, latency % 10);
    ssd1306_SetCursor(0, 54);
    ssd1306_WriteString((char*) oled_buffer, Font_7x10, White);
    ssd1306_UpdateScreen();
}