/*
 * cartridge.c
 *
 *  Created on: Nov 10, 2024
 *      Author: josh
 */

#include "cartridge.h"
#include "mapper.h"
#include "data_bus.h"

cartridge_status_t cartridge_init(cartridge_t *cart, uint8_t mapper_id, uint16_t prg_size, uint16_t chr_size, uint16_t ram_size) {

    data_bus_init(&cart->config);
    cart->status = mapper_init(&cart->mapper, mapper_id);
    if (cart->status != MAPPER_OK) {
        return CART_ERROR;
    }

    if (!mapper_validate_prg_size(prg_size)) {
        return CART_INVALID_SIZE;
    } else {
        cart->prg_size = prg_size;
    }

    if (!mapper_validate_chr_size(chr_size)) {
        return CART_INVALID_SIZE;
    } else {
        cart->chr_size = chr_size;
    }

    if (!mapper_validate_ram_size(ram_size)) {
        return CART_INVALID_SIZE;
    } else {
        cart->ram_size = ram_size;
    }

    cart->prg_bank = 0;
    cart->chr_bank = 0;
    cart->ram_bank = 0;

    return CART_OK;
}

cartridge_status_t cartridge_switch_prg_bank(cartridge_t *cart, uint8_t bank) {
    if (mapper_switch_prg_bank(&cart->mapper, &cart->config, cart->prg_size, bank) != MAPPER_OK) {
        return CART_ERROR;
    } else {
        cart->prg_bank = bank;
        return CART_OK;
    }
}

cartridge_status_t cartridge_switch_chr_bank(cartridge_t *cart, uint8_t bank) {
    if (mapper_switch_chr_bank(&cart->mapper, &cart->config, cart->chr_size, bank) != MAPPER_OK) {
        return CART_ERROR;
    } else {
        cart->chr_bank = bank;
        return CART_OK;
    }
}
