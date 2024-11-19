/*
 * cartridge.h
 *
 *  Created on: Sep 17, 2024
 *      Author: josh
 */

#ifndef INC_DATA_BUS_H_
#define INC_DATA_BUS_H_

#include "main.h"
#include "sr_74hc595.h"

#define CART_READ_MODE 0
#define CART_WRITE_MODE 1
#define DATA_BUS_CLOCK_HIGH 1
#define DATA_BUS_CLOCK_LOW 0
#define DATA_BUS_ROMSEL_ACTIVE 0
#define DATA_BUS_ROMSEL_INACTIVE 1
#define DATA_BUS_PPU_RD_ACTIVE 0
#define DATA_BUS_PPU_RD_INACTIVE 1
#define DATA_BUS_PPU_WR_ACTIVE 0
#define DATA_BUS_PPU_WR_INACTIVE 1

typedef struct {
    sr_74hc595_config_t prg_config;
    sr_74hc595_config_t chr_config;
} data_bus_config_t;


void data_bus_init(data_bus_config_t *config);
void data_bus_reset(data_bus_config_t *config);
void data_bus_set_prg_address(data_bus_config_t *config, uint16_t address);
void data_bus_set_chr_address(data_bus_config_t *config, uint16_t address);
void data_bus_set_gpio_read(GPIO_TypeDef *port, uint16_t pin);
void data_bus_set_gpio_read(GPIO_TypeDef *port, uint16_t pin);
void data_bus_set_prg_data_mode(data_bus_config_t *config, uint8_t mode);
void data_bus_set_chr_data_mode(data_bus_config_t *config, uint8_t mode);
void data_bus_write_prg_data(data_bus_config_t *config, uint8_t data);
uint8_t data_bus_read_prg_data(data_bus_config_t *config);
uint8_t data_bus_read_prg(data_bus_config_t *config, uint16_t address);
void data_bus_read_prg_block(data_bus_config_t *config, uint16_t address, uint8_t *data, uint16_t size);
void data_bus_read_chr_block(data_bus_config_t *config, uint16_t address, uint8_t *data, uint16_t size);
void data_bus_write_chr_data(data_bus_config_t *config, uint8_t data);
void data_bus_write_prg(data_bus_config_t *config, uint16_t address, uint8_t data);
void data_bus_write_reg(data_bus_config_t *config, uint16_t address, uint8_t data);
uint8_t data_bus_read_chr_data(data_bus_config_t *config);
void data_bus_configure_nes_bus(data_bus_config_t *config);
void data_bus_configure_all_output(data_bus_config_t *config);
void data_bus_configure_all_input(data_bus_config_t *config);
void data_bus_test_output_bus(data_bus_config_t *config, uint8_t position);

#endif /* INC_DATA_BUS_H_ */
