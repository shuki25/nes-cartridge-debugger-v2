/*
 * 74hc595.c
 *
 *  Created on: Sep 17, 2024
 *      Author: josh
 */

#include "sr_74hc595.h"

void sr_74hc595_init(sr_74hc595_config_t *config) {
    HAL_GPIO_WritePin(config->clock_port, config->clock_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(config->data_port, config->data_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(config->latch_port, config->latch_pin, GPIO_PIN_RESET);
    sr_74hc595_reset(config);
}

void sr_74hc595_reset(sr_74hc595_config_t *config) {
    HAL_GPIO_WritePin(config->reset_port, config->reset_pin, GPIO_PIN_RESET); // Reset the shift register by setting the reset pin to low
    HAL_GPIO_WritePin(config->reset_port, config->reset_pin, GPIO_PIN_SET);
}

void sr_74hc595_write_half_word(sr_74hc595_config_t *config, uint16_t data) {
//    for (uint8_t i = 0; i < 16; i++) {
//        HAL_GPIO_WritePin(config->clock_port, config->clock_pin, GPIO_PIN_RESET); // Set the clock pin to low
//        HAL_GPIO_WritePin(config->data_port, config->data_pin, (data & 0x01)); // Write the data to the data pin
//        HAL_GPIO_WritePin(config->clock_port, config->clock_pin, GPIO_PIN_SET); // Set the clock pin to high
//        data >>= 1;
//    }
//    HAL_GPIO_WritePin(config->clock_port, config->clock_pin, GPIO_PIN_RESET); // Set the clock pin to low
//    HAL_GPIO_WritePin(config->latch_port, config->latch_pin, GPIO_PIN_SET); // Set the latch pin to high
//    HAL_GPIO_WritePin(config->latch_port, config->latch_pin, GPIO_PIN_RESET); // Set the latch pin to low

    for (uint8_t i = 0; i < 16; i++) {
        GPIOC->ODR &= ~(1 << 11); // Set the clock pin to low
        if (data & 0x01) {
            GPIOB->ODR |= (1 << 4); // Set the data pin to high
        } else {
            GPIOB->ODR &= ~(1 << 4); // Set the data pin to low
        }
        GPIOC->ODR |= (1 << 11); // Set the clock pin to high
        data >>= 1;
    }
    GPIOC->ODR &= ~(1 << 11); // Set the clock pin to low
    GPIOC->ODR |= (1 << 10); // Set the latch pin to high
    GPIOC->ODR &= ~(1 << 10); // Set the latch pin to low

}

void sr_74hc595_write_byte(sr_74hc595_config_t *config, uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(config->clock_port, config->clock_pin, GPIO_PIN_RESET); // Set the clock pin to low
        HAL_GPIO_WritePin(config->data_port, config->data_pin,
                (data & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET); // Write the data to the data pin
        HAL_GPIO_WritePin(config->clock_port, config->clock_pin, GPIO_PIN_SET); // Set the clock pin to high
    }
    HAL_GPIO_WritePin(config->latch_port, config->latch_pin, GPIO_PIN_RESET); // Set the latch pin to low
    HAL_GPIO_WritePin(config->latch_port, config->latch_pin, GPIO_PIN_SET); // Set the latch pin to high
}
