/*
 * cartridge.h
 *
 *  Created on: Nov 10, 2024
 *      Author: josh
 */

#ifndef INC_CARTRIDGE_H_
#define INC_CARTRIDGE_H_

#include <stdint.h>
#include "mapper.h"
#include "data_bus.h"

typedef enum {
    CART_OK = 0, CART_ERROR, CART_NOT_FOUND, CART_INVALID_SIZE, CART_INVALID_BANK
} cartridge_status_t;

typedef struct {
    data_bus_config_t config;
    uint16_t chr_address;
    uint16_t prg_address;
    mapper_t mapper;
    mapper_status_t status;
    uint16_t prg_size;
    uint16_t chr_size;
    uint16_t ram_size;
    uint8_t prg_bank;
    uint8_t chr_bank;
    uint8_t ram_bank;
} cartridge_t;

cartridge_status_t cartridge_init(cartridge_t *cart, uint8_t mapper_id, uint16_t prg_size, uint16_t chr_size, uint16_t ram_size);

#endif /* INC_CARTRIDGE_H_ */
