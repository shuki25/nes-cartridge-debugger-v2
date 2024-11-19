/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "splash.h"
#include "util.h"
#include "ssd1306.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>
#include "data_bus.h"
#include "mapper.h"
#include "cartridge.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern TIM_HandleTypeDef htim4;
extern SPI_HandleTypeDef hspi1;

extern uint8_t i2c_interrupt_flag;
extern uint8_t button_interrupt_flag;
extern uint8_t button_select_flag;
extern uint8_t button_prev_flag;
extern uint8_t button_next_flag;
extern char *pin_label[];
extern char *mapper_name[];
extern uint8_t rom_data_block[];

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ledManagerTask */
osThreadId_t ledManagerTaskHandle;
const osThreadAttr_t ledManagerTask_attributes = {
  .name = "ledManagerTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartLEDManager(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of ledManagerTask */
  ledManagerTaskHandle = osThreadNew(StartLEDManager, NULL, &ledManagerTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */

    char output[64];
//    tca9555_t io_expander[4];
//    uint8_t io_expander_address[4] = { 0x20, 0x21, 0x22, 0x24 };
//    data_bus_t data_bus;
    data_bus_config_t data_bus = { 0 };
    cartridge_t cart;
    uint8_t data_bus_position = 0;
    uint8_t test_phase = 0;
    uint8_t prev_test_phase = 0;
    uint8_t detected_change = 0;
    uint16_t rom_address = 0;
    uint16_t ppu_address = 0;
    uint8_t rom_dump_flag = 0;
    uint32_t start_time = 0;
    uint32_t end_time = 0;
    uint32_t elapsed_time = 0;
    uint32_t bytes_per_second = 0;
    char *mapper_name;

    // Initialize the OLED screen
    splash();
    ssd1306_Fill(Black);
    ssd1306_SetCursor(22, 20);
    ssd1306_WriteString("Press Select", Font_7x10, White);
    ssd1306_SetCursor(36, 32);
    ssd1306_WriteString("to Start", Font_7x10, White);
    ssd1306_UpdateScreen();

    // Set up data bus and shift registers
    data_bus_init(&data_bus);
    data_bus_configure_all_output(&data_bus);

    // Initialize the cartridge
    if (cartridge_init(&cart, 1, 32, 16, 0) != CART_OK) {
        sprintf((char*) output, "Error initializing cartridge\n");
        print_terminal((char*) output);
    } else {
        sprintf((char*) output, "Cartridge initialized\n");
        print_terminal((char*) output);
    }

    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
    /* Infinite loop */
    for (;;) {
        if (button_interrupt_flag) {
            button_interrupt_flag = 0;
            if (button_select_flag) {
                sprintf((char*) output, "OK\tSelect Button Pressed!\n");
                print_terminal((char*) output);
                button_select_flag = 0;
                if (test_phase == 0) {
                    if (data_bus_position >= 54) {
                        data_bus_test_output_bus(&data_bus, data_bus_position); // Clear last pin
                        data_bus_position = 0;
                    }
                    sprintf((char*) output, "Testing %s", pin_label[data_bus_position]);
                    ssd1306_Fill(Black);
                    ssd1306_WriteStringCentered((char*) output, Font_7x10, White, 27);
                    ssd1306_UpdateScreen();
                    data_bus_test_output_bus(&data_bus, data_bus_position);
                    data_bus_position++;
                } else if (test_phase == 1) {

                    ssd1306_Fill(Black);
                    ssd1306_WriteStringCentered("Running Clock", Font_7x10, White, 20);
                    ssd1306_WriteStringCentered("Frequency Test", Font_7x10, White, 32);
                    ssd1306_UpdateScreen();

//                    data_bus_clk_test(&data_bus);

                    ssd1306_WriteStringCentered("Completed", Font_7x10, White, 50);
                    ssd1306_UpdateScreen();
                } else if (test_phase == 2) {
                    ssd1306_Fill(Black);
                    ssd1306_WriteStringCentered("Waiting for Input", Font_7x10, White, 20);
                    ssd1306_UpdateScreen();
//                    data_bus_test_read(&data_bus, &data_bus_position);
                } else if (test_phase == 3) {
                    ssd1306_Fill(Black);
                    ssd1306_WriteStringCentered("ROM Test", Font_7x10, White, 20);
                    ssd1306_UpdateScreen();
                    switch (rom_address) {
                    case 0x8000:
                        rom_address = 0x8100;
                        ppu_address = 0x0100;
                        break;
                    case 0x8100:
                        rom_address = 0xC000;
                        ppu_address = 0x1000;
                        break;
                    case 0xC000:
                        rom_address = 0xFF00;
                        ppu_address = 0x2000;
                        break;
                    default:
                        rom_address = 0x8000;
                        ppu_address = 0x0000;
                        break;
                    }
                    rom_dump_flag = 0;
                }
            } else if (button_prev_flag) {
                sprintf((char*) output, "OK\tPrev Button Pressed!\n");
                print_terminal((char*) output);
                button_prev_flag = 0;
                if (test_phase == 0) {
                    test_phase = 3;
                } else {
                    test_phase--;
                }
            } else if (button_next_flag) {
                sprintf((char*) output, "OK\tNext Button Pressed!\n");
                print_terminal((char*) output);
                button_next_flag = 0;
                if (test_phase == 3) {
                    test_phase = 0;
                } else {
                    test_phase++;
                }
            }
            if (prev_test_phase != test_phase) {
                prev_test_phase = test_phase;
                ssd1306_Fill(Black);
                if (test_phase == 0) {
                    ssd1306_WriteStringCentered("Individual Pin", Font_7x10, White, 20);
                    ssd1306_WriteStringCentered("Output Test", Font_7x10, White, 32);
                    data_bus_configure_all_output(&data_bus);
                    data_bus_position = 0;

                } else if (test_phase == 1) {
                    ssd1306_WriteStringCentered("Clock Frequency", Font_7x10, White, 10);
                    ssd1306_WriteStringCentered("Frequency Test", Font_7x10, White, 22);
                } else if (test_phase == 2) {
                    ssd1306_WriteStringCentered("Individual Pin", Font_7x10, White, 20);
                    ssd1306_WriteStringCentered("Input Test", Font_7x10, White, 32);
                    data_bus_set_prg_data_mode(&data_bus, 0);
                    data_bus_set_chr_data_mode(&data_bus, 0);
                    data_bus_set_gpio_read(CIRAM_A10_GPIO_Port, CIRAM_A10_Pin);
                } else if (test_phase == 3) {
                    ssd1306_WriteStringCentered("ROM Test", Font_7x10, White, 20);
                    rom_address = 0x8000;
                    ppu_address = 0x0000;
                    data_bus_configure_nes_bus(&data_bus);
                    rom_dump_flag = 0;
                }
                ssd1306_UpdateScreen();
            }
        }
        if (i2c_interrupt_flag) {
            i2c_interrupt_flag = 0;
            sprintf((char*) output, "OK\tI2C Interrupt!\n");
            print_terminal((char*) output);
            detected_change = 1;
        }
        if (test_phase == 2 && detected_change) {
//            data_bus_test_read(&data_bus, &data_bus_position);
            if (data_bus_position != 255) {
                ssd1306_WriteStringCentered("                  ", Font_7x10, White, 32);
//                sprintf((char*) output, "Detected %s", pin_label[data_bus_position]);
                ssd1306_WriteStringCentered((char*) output, Font_7x10, White, 32);
                ssd1306_UpdateScreen();
            } else {
                //sprintf((char*) output, "No Input Detected");
            }

            detected_change = 0;
        }
        if (test_phase == 3 && !rom_dump_flag) {
            // TODO: Add test for getting data from ROM
//            rom_data_block[0] = data_bus_read_prg(&data_bus, rom_address);
            start_time = TIM2->CNT;
            data_bus_read_prg_block(&data_bus, rom_address, rom_data_block, 256);
            end_time = TIM2->CNT;
            elapsed_time = end_time - start_time;
            bytes_per_second = calculate_data_rate(256, elapsed_time);
            sprintf((char*) output, "Elapsed Time: %ld us, %ld ms\n", elapsed_time, (elapsed_time/1000));
            print_terminal((char*) output);
            sprintf((char*) output, "Data Rate: %ld Bps, %ld KBps\n", bytes_per_second, (bytes_per_second/1024));
            print_terminal((char*) output);
            sprintf((char*) output, "PRG ROM Dump\n");
            print_terminal((char*) output);
            print_hex(rom_data_block, 256, 16, rom_address);
            rom_dump_flag = 1;
//            data_bus_cpu_read(&data_bus, rom_address);
            ssd1306_Fill(Black);
            mapper_name = mapper_get_name(cart.mapper.mapper_id);
            sprintf((char*) output, "Mapper:%s", mapper_name);
            ssd1306_WriteStringCentered((char*) output, Font_7x10, White, 10);
            sprintf((char*) output, "PRG %04X", rom_address);
            ssd1306_WriteStringCentered((char*) output, Font_7x10, White, 32);
            ssd1306_UpdateScreen();
            sprintf((char*) output, "CHR ROM Dump\n");
            print_terminal((char*) output);
            data_bus_read_chr_block(&data_bus, ppu_address, rom_data_block, 256);
            print_hex(rom_data_block, 256, 16, ppu_address);
//            rom_address++;
//            if (rom_address > 0x807F) {
//                rom_address = 0x8000;
//            }
//            ppu_address++;
//            if (ppu_address > 0x27FF) {
//                ppu_address = 0x2000;
//            }
//            osDelay(200);
        }
        osThreadYield();
    }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartLEDManager */
/**
 * @brief Function implementing the ledManagerTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartLEDManager */
void StartLEDManager(void *argument)
{
  /* USER CODE BEGIN StartLEDManager */
    uint8_t led_state = 0;
    uint16_t counter = 0;

    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3); // Blue LED - NES Clock
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4); // Green LED - Status
    /* Infinite loop */
    for (;;) {
        led_state = !led_state;
        if (led_state && counter > 1000) {
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 65535 * .9);  // 50% duty cycle
            counter = 0;

        } else if (!led_state && counter > 1000) {
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0);
            counter = 0;
        }

        if (button_interrupt_flag) {
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 65535 * .1); //20% duty cycle
        } else {
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
        }

        counter++;
        osDelay(1);
    }
  /* USER CODE END StartLEDManager */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

