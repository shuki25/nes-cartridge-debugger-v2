/*
 * mapper.c
 *
 *  Created on: Nov 10, 2024
 *      Author: josh
 */

#include "mapper.h"

// @formatter:off

// mapper_id, prg_low, prg_high, chr_low, chr_high, ram_low, ram_high

static const mapper_t mapper_NES[] = {
    { 0, 0, 0, 0, 0, 0, 0 }, // NROM
    { 1, 1, 5, 0, 5, 0, 3 }, // MMC1
    { 4, 1, 5, 0, 6, 0, 1 } // MMC3
};

uint8_t mapper_count = sizeof(mapper_NES) / sizeof(mapper_t);

static char *mapper_name[] = {
    "NROM\0", "MMC1\0", "UxROM\0", "CNROM\0", "MMC3\0"
};

static const uint16_t prog_ROM_size[] = {
   16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
};
uint16_t min_prg_ROM_size = 0;
uint16_t max_prg_ROM_size = sizeof(prog_ROM_size) / sizeof(uint16_t) - 1;

static const uint16_t chr_ROM_size[] = {
   8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096
};
uint16_t min_chr_ROM_size = 0;
uint16_t max_chr_ROM_size = sizeof(chr_ROM_size) / sizeof(uint16_t) - 1;

static const uint16_t ram_RAM_size[] = {
   0, 8, 16, 32
};
uint16_t min_ram_size = 0;
uint16_t max_ram_size = sizeof(ram_RAM_size) / sizeof(uint16_t) - 1;

// @formatter:on

/*-----------------------------------------------------------------------------
 * Function: mapper_get_name
 *
 * This function returns the mapper name
 *
 * Parameters: uint8_t mapper_id - mapper ID
 * Return: char * - mapper name
 *---------------------------------------------------------------------------*/
char* mapper_get_name(uint8_t mapper_id) {
    return mapper_name[mapper_id];
}

/*-----------------------------------------------------------------------------
 * Function: mapper_init
 *
 * This function initializes the mapper configuration
 *
 * Parameters: mapper_t *mapper - mapper configuration
 *             uint8_t mapper_id - mapper ID
 * Return: None
 *---------------------------------------------------------------------------*/
mapper_status_t mapper_init(mapper_t *mapper, uint8_t mapper_id) {

    for (uint8_t i = 0; i < mapper_count; i++) {
        if (mapper_NES[i].mapper_id == mapper_id) {
            mapper->mapper_id = mapper_id;
            mapper->prg_low = mapper_NES[i].prg_low;
            mapper->prg_high = mapper_NES[i].prg_high;
            mapper->chr_low = mapper_NES[i].chr_low;
            mapper->chr_high = mapper_NES[i].chr_high;
            mapper->ram_low = mapper_NES[i].ram_low;
            mapper->ram_high = mapper_NES[i].ram_high;
            return MAPPER_OK;
        }
    }
    return MAPPER_NOT_FOUND;

    return MAPPER_OK;
}

/*-----------------------------------------------------------------------------
 * Function: mapper_validate_prg_size
 *
 * This function validates the PRG ROM size
 *
 * Parameters: uint16_t size - PRG ROM size
 * Return: uint8_t - 1 if valid, 0 if invalid
 *---------------------------------------------------------------------------*/
uint8_t mapper_validate_prg_size(uint16_t size) {
    for (uint8_t i = 0; i < max_prg_ROM_size; i++) {
        if (prog_ROM_size[i] == size) {
            return 1;
        }
    }
    return 0;
}

/*-----------------------------------------------------------------------------
 * Function: mapper_validate_chr_size
 *
 * This function validates the CHR ROM size
 *
 * Parameters: uint16_t size - CHR ROM size
 * Return: uint8_t - 1 if valid, 0 if invalid
 *---------------------------------------------------------------------------*/
uint8_t mapper_validate_chr_size(uint16_t size) {
    for (uint8_t i = 0; i < mapper_count; i++) {
        if (chr_ROM_size[i] == size) {
            return 1;
        }
    }
    return 0;
}

/*-----------------------------------------------------------------------------
 * Function: mapper_validate_ram_size
 *
 * This function validates the RAM size
 *
 * Parameters: uint16_t size - RAM size
 * Return: uint8_t - 1 if valid, 0 if invalid
 *---------------------------------------------------------------------------*/
uint8_t mapper_validate_ram_size(uint16_t size) {
    for (uint8_t i = 0; i < mapper_count; i++) {
        if (ram_RAM_size[i] == size) {
            return 1;
        }
    }
    return 0;
}

/*-----------------------------------------------------------------------------
 * Function: mapper_write_mmc1_byte
 *
 * This function writes a byte to the MMC1 mapper
 * This is a fix for MMC1 RAM corruption
 *
 * Parameters: data_bus_config_t *data_bus - data bus configuration
 *            uint16_t address - address to write to
 *            uint8_t data - data to write
 *
 * Return: None
 *---------------------------------------------------------------------------*/
void mapper_write_mmc1_byte(data_bus_config_t *data_bus, uint16_t address, uint8_t data) {
    if (address >= 0xE000) {
        for (uint8_t i = 0; i < 5; i++) {
            data_bus_write_reg(data_bus, address, data >> i);
        }
    } else {
        for (uint8_t i = 0; i < 5; i++) {
            data_bus_write_prg(data_bus, address, data >> i);
        }
    }
}

/*-----------------------------------------------------------------------------
 * Function: mapper_switch_bank
 *
 * This function switches the bank for the mapper
 *
 * Parameters: mapper_t *mapper - mapper configuration
 *            data_bus_config_t *data_bus - data bus configuration
 *             uint8_t bank - bank number
 * Return: None
 *---------------------------------------------------------------------------*/
mapper_status_t mapper_switch_prg_bank(mapper_t *mapper, data_bus_config_t *data_bus, uint16_t prg_size,
        uint8_t bank) {
    uint8_t bank_size = 0;
    uint8_t bank_max = 0;

    switch (mapper->mapper_id) {

    case 0: // NROM
        if (bank == 0) {
            return MAPPER_OK;
        }
        break;
    case 1: // MMC1
        bank_size = 16;
        bank_max = prg_size / bank_size;

        if (bank > bank_max) {
            return MAPPER_INVALID_BANK;
        }
        // TO DO: Implement bank switching with data bus
        return MAPPER_OK;
        break;

    }

    return MAPPER_ERROR;
}

/*-----------------------------------------------------------------------------
 * Function: mapper_switch_chr_bank
 *
 * This function switches the bank for the mapper
 *
 * Parameters: mapper_t *mapper - mapper configuration
 *             data_bus_config_t *data_bus - data bus configuration
 *             uint8_t bank - bank number
 * Return: None
 *---------------------------------------------------------------------------*/
mapper_status_t mapper_switch_chr_bank(mapper_t *mapper, data_bus_config_t *data_bus, uint16_t chr_size,
        uint8_t bank) {
    uint8_t bank_size = 0;
    uint8_t bank_max = 0;

    switch (mapper->mapper_id) {

    case 0: // NROM
        if (bank == 0) {
            return MAPPER_OK;
        }
        break;
    case 1: // MMC1
        bank_size = 8;
        bank_max = chr_size / bank_size;

        if (bank > bank_max) {
            return MAPPER_INVALID_BANK;
        }
        // TO DO: Implement bank switching with data bus
        return MAPPER_OK;
        break;

    }

    return MAPPER_ERROR;
}

mapper_status_t mapper_dump_prg(mapper_t *mapper, data_bus_config_t *data_bus, uint16_t start_address,
        uint8_t *data, uint16_t *data_size) {

    switch (mapper->mapper_id) {

    case 0: // NROM
        for (uint16_t i = 0; i < *data_size; i++) {
            data[i] = data_bus_read_prg(data_bus, start_address + i);
        }
        return MAPPER_OK;
        break;
    case 1: // MMC1
        for (uint16_t i = 0; i < *data_size; i++) {
            data[i] = data_bus_read_prg(data_bus, start_address + i);
        }
        return MAPPER_OK;
        break;

    }

    return MAPPER_ERROR;
}
