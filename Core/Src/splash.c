/*
 * splash.c
 *
 *  Created on: Dec 31, 2023
 *      Author: Joshua Butler, MD, MHI
 */

#include <stdio.h>
#include <string.h>
#include "splash_bitmap.h"
#include "splash.h"
#include "main.h"

void splash(void) {

	uint8_t buffer[12];

	ssd1306_Init();
	ssd1306_SetContrast(20);
	ssd1306_Fill(Black);
	ssd1306_SetCursor(16, 1);
	ssd1306_WriteString("BUTLER", Font_16x26, White);
	ssd1306_SetCursor(47, 28);
	ssd1306_WriteString("TEK", Font_11x18, White);
	ssd1306_SetCursor(0, 53);
	ssd1306_WriteString("(C)2025", Font_7x10, White);
	snprintf((char*) buffer, 12, "v%d.%d.%d%s", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, BOARD_REVISION);
	uint8_t len = strlen((char*) buffer);
	ssd1306_SetCursor(128 - (len * 7), 53);
	ssd1306_WriteString((char*) buffer, Font_7x10, White);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
	ssd1306_Fill(Black);
	ssd1306_DrawBitmap(0, 0, bitmap_splash, 128, 64, White);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
}

void draw_home_screen(void) {

    ssd1306_Init();
    ssd1306_UpdateScreen();
//    ssd1306_SetContrast(20);
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
}
