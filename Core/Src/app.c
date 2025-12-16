/*
 * nes-cartridge-debugger-v2
 * 
 * Copyright (c) 2025 Dr. Joshua Butler
 * All rights reserved.

 * app.c
 *
 *  Created on: 15 Dec 2025
 *      Author: josh
 */

#include "app.h"
#include <string.h>
#include "main.h"
#include "debug.h"
#include "ssd1306.h"
#include "splash.h"

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5;

/**
 * @brief Initialize the main application structure
 * @param app: Pointer to the application structure
 * @return None
 */
void app_init(app_t *app) {
	memset(app, 0, sizeof(app_t));
	app->state_machine.current_state = STATE_INITIALIZE;

	// Initialize LEDs
	led_init_pwm(&app->nes_clk_led, &htim4, TIM_CHANNEL_3, &htim5); // Blue LED - NES Clock
	led_init_pwm(&app->status_led, &htim4, TIM_CHANNEL_4, &htim5); // Green LED - Status
	led_set_mode(&app->nes_clk_led, LED_BLINK_CONTINUOUS);
	led_set_mode(&app->status_led, LED_FADE_CONTINUOUS);
	led_set_blink_delay(&app->nes_clk_led, 500, 500);
	led_set_blink_delay(&app->status_led, 2000, 2000);
	led_set_pwm_max_duty(&app->nes_clk_led, 100); // 100% duty cycle
	led_set_pwm_max_duty(&app->status_led, 45); // 45% duty cycle

	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3); // Blue LED - NES Clock
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4); // Green LED - Status
	HAL_TIM_Base_Start(&htim5); // Activate Delay Timer

	// Initialize Data Bus
	data_bus_init(&app->data_bus);
	data_bus_configure_all_output(&app->data_bus);

	// Initialize Cartridge
	if (cartridge_init(&app->cart, 1, 32, 16, 0) != CART_OK) {
		// Handle cartridge initialization error
		PRINT_DEBUG("ERROR: Failed to initialize cartridge\n");
	} else {
		PRINT_DEBUG("INFO: Cartridge initialized successfully\n");
	}

}

/**
 * @brief Update the application state machine
 * @param app: Pointer to the application structure
 * @return None
 */
void app_loop(app_t *app) {
	switch (app->state_machine.current_state) {
	case STATE_IDLE:
		// Handle idle state
		break;
	case STATE_ERROR:
		// Handle error state
		break;
	case STATE_INITIALIZE:
		// Handle initialization state
		splash();
		ssd1306_Fill(Black);
		ssd1306_SetCursor(22, 20);
		ssd1306_WriteString("Press Select", Font_7x10, White);
		ssd1306_SetCursor(36, 32);
		ssd1306_WriteString("to Start", Font_7x10, White);
		ssd1306_UpdateScreen();
		app->state_machine.current_state = STATE_WAIT_FOR_COMMAND;

	case STATE_SPLASH_SCREEN:
		// Handle splash screen state
	case STATE_TEST_OUTPUT_BUS:
		// Handle test output bus state
		break;
	case STATE_TEST_INPUT_BUS:
		// Handle test input bus state
		break;
	case STATE_READ_PRG_ROM:
		// Handle read PRG ROM state
		break;
	case STATE_READ_CHR_ROM:
		// Handle read CHR ROM state
		break;
	case STATE_PARSE_COMMAND:
		// Handle parse command state
		break;
	case STATE_WAIT_FOR_COMMAND:
		// Handle wait for command state
		// TODO: Check for button press to transition to execute command state

		// TODO: Check for command input (e.g., from UART or other interface)

		break;
	case STATE_EXECUTE_COMMAND:
		// Handle execute command state
		break;
	default:
		break;
	}

	// Update LEDs based on state
	led_indicator(&app->nes_clk_led);
	led_indicator(&app->status_led);
}
