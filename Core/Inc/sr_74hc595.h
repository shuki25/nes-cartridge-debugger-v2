/*
 * 74hc595.h
 *
 *  Created on: Sep 17, 2024
 *      Author: josh
 */

#ifndef INC_SR_74HC595_H_
#define INC_SR_74HC595_H_

#include "main.h"

typedef struct {
    GPIO_TypeDef *clock_port;
    uint16_t clock_pin;
    GPIO_TypeDef *data_port;
    uint16_t data_pin;
    GPIO_TypeDef *latch_port;
    uint16_t latch_pin;
    GPIO_TypeDef *reset_port;
    uint16_t reset_pin;
} sr_74hc595_config_t;


void sr_74hc595_init(sr_74hc595_config_t *config);
void sr_74hc595_reset(sr_74hc595_config_t *config);
void sr_74hc595_write_half_word(sr_74hc595_config_t *config, uint16_t data);
void sr_74hc595_write_byte(sr_74hc595_config_t *config, uint8_t data);

#endif /* INC_SR_74HC595_H_ */
