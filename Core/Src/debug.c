/*
 * nes-cartridge-debugger-v2
 * 
 * Copyright (c) 2025 Dr. Joshua Butler
 * All rights reserved.

 * debug.c
 *
 *  Created on: 16 Dec 2025
 *      Author: josh
 */


#include "debug.h"
#include "main.h"

int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        ITM_SendChar((*ptr++));
    }
    return len;
}
