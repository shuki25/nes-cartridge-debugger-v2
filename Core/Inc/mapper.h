/*
 * mapper.h
 *
 *  Created on: Nov 10, 2024
 *      Author: josh
 */

#ifndef INC_MAPPER_H_
#define INC_MAPPER_H_

#include <data_bus.h>
#include <stdint.h>

typedef enum {
    MAPPER_OK = 0, MAPPER_ERROR, MAPPER_NOT_FOUND, MAPPER_INVALID_SIZE, MAPPER_INVALID_BANK
} mapper_status_t;

typedef struct {
    uint8_t mapper_id;
    uint8_t prg_low;
    uint8_t prg_high;
    uint8_t chr_low;
    uint8_t chr_high;
    uint8_t ram_low;
    uint8_t ram_high;
} mapper_t;

char* mapper_get_name(uint8_t mapper_id);
mapper_status_t mapper_init(mapper_t *mapper, uint8_t mapper_id);
mapper_status_t mapper_switch_prg_bank(mapper_t *mapper, data_bus_config_t *data_bus, uint16_t prg_size, uint8_t bank);
mapper_status_t mapper_switch_chr_bank(mapper_t *mapper, data_bus_config_t *data_bus, uint16_t chr_size, uint8_t bank);
uint8_t mapper_validate_prg_size(uint16_t size);
uint8_t mapper_validate_chr_size(uint16_t size);
uint8_t mapper_validate_ram_size(uint16_t size);

#endif /* INC_MAPPER_H_ */
