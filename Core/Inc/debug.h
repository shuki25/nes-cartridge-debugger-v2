/*
 * nes-cartridge-debugger-v2
 * 
 * Copyright (c) 2025 Dr. Joshua Butler
 * All rights reserved.

 * debug.h
 *
 *  Created on: 15 Dec 2025
 *      Author: josh
 */

#ifndef INC_DEBUG_H_
#define INC_DEBUG_H_

#ifdef DEBUG
#define PRINT_FMT_DEBUG(fmt, ...) printf(fmt, __VA_ARGS__)
#define PRINT_DEBUG(str) printf(str)
#else
#define PRINT_FMT_DEBUG(x)
#define PRINT_DEBUG(x)
#endif

#include <stdio.h>

#endif /* INC_DEBUG_H_ */
