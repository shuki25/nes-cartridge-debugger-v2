/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OLED_CS_Pin GPIO_PIN_13
#define OLED_CS_GPIO_Port GPIOC
#define OLED_DC_Pin GPIO_PIN_14
#define OLED_DC_GPIO_Port GPIOC
#define OLED_RST_Pin GPIO_PIN_15
#define OLED_RST_GPIO_Port GPIOC
#define CPU_D7_Pin GPIO_PIN_0
#define CPU_D7_GPIO_Port GPIOC
#define CPU_D6_Pin GPIO_PIN_1
#define CPU_D6_GPIO_Port GPIOC
#define CPU_D5_Pin GPIO_PIN_2
#define CPU_D5_GPIO_Port GPIOC
#define CPU_IRQ_Pin GPIO_PIN_3
#define CPU_IRQ_GPIO_Port GPIOC
#define CPU_D4_Pin GPIO_PIN_0
#define CPU_D4_GPIO_Port GPIOA
#define CPU_D3_Pin GPIO_PIN_1
#define CPU_D3_GPIO_Port GPIOA
#define CPU_D2_Pin GPIO_PIN_2
#define CPU_D2_GPIO_Port GPIOA
#define CPU_D1_Pin GPIO_PIN_3
#define CPU_D1_GPIO_Port GPIOA
#define CPU_D0_Pin GPIO_PIN_4
#define CPU_D0_GPIO_Port GPIOA
#define PPU_RD_Pin GPIO_PIN_6
#define PPU_RD_GPIO_Port GPIOA
#define ROMSEL_Pin GPIO_PIN_4
#define ROMSEL_GPIO_Port GPIOC
#define CPU_RW_Pin GPIO_PIN_5
#define CPU_RW_GPIO_Port GPIOC
#define M2_Pin GPIO_PIN_0
#define M2_GPIO_Port GPIOB
#define PPU_WR_Pin GPIO_PIN_1
#define PPU_WR_GPIO_Port GPIOB
#define CIRAM_CE_Pin GPIO_PIN_2
#define CIRAM_CE_GPIO_Port GPIOB
#define PPU_ADDR_Pin GPIO_PIN_10
#define PPU_ADDR_GPIO_Port GPIOB
#define PPU_STR_CLK_Pin GPIO_PIN_12
#define PPU_STR_CLK_GPIO_Port GPIOB
#define PPU_SR_CLK_Pin GPIO_PIN_13
#define PPU_SR_CLK_GPIO_Port GPIOB
#define CIRAM_A10_Pin GPIO_PIN_14
#define CIRAM_A10_GPIO_Port GPIOB
#define PPU_D7_Pin GPIO_PIN_15
#define PPU_D7_GPIO_Port GPIOB
#define PPU_D6_Pin GPIO_PIN_6
#define PPU_D6_GPIO_Port GPIOC
#define PPU_D5_Pin GPIO_PIN_7
#define PPU_D5_GPIO_Port GPIOC
#define PPU_D4_Pin GPIO_PIN_8
#define PPU_D4_GPIO_Port GPIOC
#define PPU_D3_Pin GPIO_PIN_9
#define PPU_D3_GPIO_Port GPIOC
#define PPU_D2_Pin GPIO_PIN_8
#define PPU_D2_GPIO_Port GPIOA
#define PPU_D1_Pin GPIO_PIN_9
#define PPU_D1_GPIO_Port GPIOA
#define PPU_D0_Pin GPIO_PIN_10
#define PPU_D0_GPIO_Port GPIOA
#define CPU_STR_CLK_Pin GPIO_PIN_10
#define CPU_STR_CLK_GPIO_Port GPIOC
#define CPU_SR_CLK_Pin GPIO_PIN_11
#define CPU_SR_CLK_GPIO_Port GPIOC
#define SR_RST_Pin GPIO_PIN_12
#define SR_RST_GPIO_Port GPIOC
#define CPU_ADDR_Pin GPIO_PIN_4
#define CPU_ADDR_GPIO_Port GPIOB
#define BTN_NEXT_Pin GPIO_PIN_5
#define BTN_NEXT_GPIO_Port GPIOB
#define BTN_NEXT_EXTI_IRQn EXTI9_5_IRQn
#define BTN_PREV_Pin GPIO_PIN_6
#define BTN_PREV_GPIO_Port GPIOB
#define BTN_PREV_EXTI_IRQn EXTI9_5_IRQn
#define BTN_SELECT_Pin GPIO_PIN_7
#define BTN_SELECT_GPIO_Port GPIOB
#define BTN_SELECT_EXTI_IRQn EXTI9_5_IRQn
#define LED_NES_CLK_Pin GPIO_PIN_8
#define LED_NES_CLK_GPIO_Port GPIOB
#define LED_HB_Pin GPIO_PIN_9
#define LED_HB_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define VERSION_MAJOR 1
#define VERSION_MINOR 2
#define VERSION_PATCH 0
#define VERSION_STRING "1.2.0"
#define BOARD_REVISION "B"

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
