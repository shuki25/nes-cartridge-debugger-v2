/*
 * cartridge.c
 *
 *  Created on: Sep 17, 2024
 *      Author: josh
 */

#include <data_bus.h>
#include <string.h>
#include "sr_74hc595.h"
#include "ui.h"
#include "main.h"
#include "debug.h"


extern TIM_HandleTypeDef htim2;

char *pin_label[] = { "CPU_A0", "CPU_A1", "CPU_A2", "CPU_A3", "CPU_A4", "CPU_A5", "CPU_A6", "CPU_A7",
        "CPU_A8", "CPU_A9", "CPU_A10", "CPU_A11", "CPU_A12", "CPU_A13", "CPU_A14", "CPU_D0", "CPU_D1",
        "CPU_D2", "CPU_D3", "CPU_D4", "CPU_D5", "CPU_D6", "CPU_D7", "PPU_A0", "PPU_A1", "PPU_A2", "PPU_A3",
        "PPU_A4", "PPU_A5", "PPU_A6", "PPU_A7", "PPU_A8", "PPU_A9", "PPU_A10", "PPU_A11", "PPU_A12",
        "PPU_A13", "PPU_A13I", "PPU_D0", "PPU_D1", "PPU_D2", "PPU_D3", "PPU_D4", "PPU_D5", "PPU_D6", "PPU_D7",
        "PPU_RD", "PPU_WR", "CPU_RW", "CPU_IRQ", "CPU_ROMSEL", "CPU_M2", "CIRAM_CE", "CIRAM_A10" };

GPIO_TypeDef *prg_data_ports[] = { CPU_D0_GPIO_Port, CPU_D1_GPIO_Port, CPU_D2_GPIO_Port, CPU_D3_GPIO_Port,
CPU_D4_GPIO_Port, CPU_D5_GPIO_Port, CPU_D6_GPIO_Port, CPU_D7_GPIO_Port };
uint16_t prg_data_pins[] = { CPU_D0_Pin, CPU_D1_Pin, CPU_D2_Pin, CPU_D3_Pin, CPU_D4_Pin, CPU_D5_Pin,
CPU_D6_Pin, CPU_D7_Pin };

GPIO_TypeDef *chr_data_ports[] = { PPU_D0_GPIO_Port, PPU_D1_GPIO_Port, PPU_D2_GPIO_Port, PPU_D3_GPIO_Port,
PPU_D4_GPIO_Port, PPU_D5_GPIO_Port, PPU_D6_GPIO_Port, PPU_D7_GPIO_Port };
uint16_t chr_data_pins[] = { PPU_D0_Pin, PPU_D1_Pin, PPU_D2_Pin, PPU_D3_Pin, PPU_D4_Pin, PPU_D5_Pin,
PPU_D6_Pin, PPU_D7_Pin };

// Reverse bit lookup table for first 3 bits
static uint8_t reverse_bits3[] = { 0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0,
        0x30, 0xB0, 0x70, 0xF0 };

// Reverse bit lookup table for first 5 bits and maintain position
static uint8_t reverse_bits5[] = { 0x00, 0x10, 0x08, 0x18, 0x04, 0x14, 0x0C, 0x1C, 0x02, 0x12, 0x0A, 0x1A,
        0x06, 0x16, 0x0E, 0x1E, 0x01, 0x11, 0x09, 0x19, 0x05, 0x15, 0x0D, 0x1D, 0x03, 0x13, 0x0B, 0x1B, 0x07,
        0x17, 0x0F, 0x1F };

// Internal functions

void _delay_us(uint32_t us) {
    return;
    uint32_t start_time = TIM2->CNT;
    uint32_t end_time = start_time + us;

    // Check for rollover, make adjustment to end time
    if (end_time < start_time) {
        end_time = us - (0xFFFFFFFF - start_time);
    }

    while (TIM2->CNT < end_time)
        ;

//    uint32_t start = TIM2->CNT; // Capture the starting counter value
//    uint32_t end = (start + us) % 0xFFFFFFFF; // Compute the target counter value
//
//    // Wait until the counter reaches the target value, considering rollover
//    while (((TIM2->CNT - start) + 0xFFFFFFFF + 1) % 0xFFFFFFFF < us)
//        ;
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_init
 *
 * This function initializes the cartridge configuration
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint8_t memory_mapper_id - memory mapper ID
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_init(data_bus_config_t *config) {
    config->prg_config.clock_port = CPU_SR_CLK_GPIO_Port;
    config->prg_config.clock_pin = CPU_SR_CLK_Pin;
    config->prg_config.data_port = CPU_ADDR_GPIO_Port;
    config->prg_config.data_pin = CPU_ADDR_Pin;
    config->prg_config.latch_port = CPU_STR_CLK_GPIO_Port;
    config->prg_config.latch_pin = CPU_STR_CLK_Pin;
    config->prg_config.reset_port = SR_RST_GPIO_Port;
    config->prg_config.reset_pin = SR_RST_Pin;

    config->chr_config.clock_port = PPU_SR_CLK_GPIO_Port;
    config->chr_config.clock_pin = PPU_SR_CLK_Pin;
    config->chr_config.data_port = PPU_ADDR_GPIO_Port;
    config->chr_config.data_pin = PPU_ADDR_Pin;
    config->chr_config.latch_port = PPU_STR_CLK_GPIO_Port;
    config->chr_config.latch_pin = PPU_STR_CLK_Pin;
    config->chr_config.reset_port = SR_RST_GPIO_Port;
    config->chr_config.reset_pin = SR_RST_Pin;

    sr_74hc595_init(&config->prg_config);
    sr_74hc595_init(&config->chr_config);
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_reset
 *
 * This function resets the cartridge configuration
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_reset(data_bus_config_t *config) {
    sr_74hc595_reset(&config->prg_config);
    sr_74hc595_reset(&config->chr_config);
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_m2
 *
 * This function sets the CPU M2 clock
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint8_t clock_state - clock state (0: low, 1: high)
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_m2(data_bus_config_t *config, uint8_t clock_state) {
    if (clock_state) {
        HAL_GPIO_WritePin(M2_GPIO_Port, M2_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(M2_GPIO_Port, M2_Pin, GPIO_PIN_RESET);
    }
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_chr_RD
 *
 * This function sets the PPU read mode
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *            uint8_t state - read mode (0: active, 1: inactive)
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_chr_RD(data_bus_config_t *config, uint8_t state) {
    if (state) {
        HAL_GPIO_WritePin(PPU_RD_GPIO_Port, PPU_RD_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(PPU_RD_GPIO_Port, PPU_RD_Pin, GPIO_PIN_RESET);
    }
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_set_prg_rw
 *
 * This function sets the CPU read/write mode
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint8_t state - read/write mode (0: write, 1: read)
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_set_prg_rw(data_bus_config_t *config, uint8_t state) {
    if (state) {
        HAL_GPIO_WritePin(CPU_RW_GPIO_Port, CPU_RW_Pin, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(CPU_RW_GPIO_Port, CPU_RW_Pin, GPIO_PIN_SET);
    }
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_set_romsel
 *
 * It is low when the CPU reads or writes to $8000-$FFFF and when
 * the address is stable, allowing to enable ROM chips directly.
 * Otherwise, it is high.
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint8_t state - ROM select state (0: select, 1: deselect)
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_set_romsel(data_bus_config_t *config, uint16_t address) {

    if (address & 0x8000) {
        HAL_GPIO_WritePin(ROMSEL_GPIO_Port, ROMSEL_Pin, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(ROMSEL_GPIO_Port, ROMSEL_Pin, GPIO_PIN_SET);
    }
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_romsel
 *
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint8_t state - ROM select state (0: select, 1: deselect)
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_romsel(data_bus_config_t *config, uint8_t state) {
    if (state) {
        HAL_GPIO_WritePin(ROMSEL_GPIO_Port, ROMSEL_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(ROMSEL_GPIO_Port, ROMSEL_Pin, GPIO_PIN_RESET);
    }
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_set_prg_address
 *
 * This function sets the CPU address for the cartridge
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint16_t address - CPU address to set for PRG ROM
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_set_prg_address(data_bus_config_t *config, uint16_t address) {
    sr_74hc595_write_half_word(&config->prg_config, address);
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_set_chr_address
 *
 * This function sets the CHR address for the cartridge
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint16_t address - PPU address to set for CHR ROM
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_set_chr_address(data_bus_config_t *config, uint16_t address) {
    sr_74hc595_write_half_word(&config->chr_config, address);
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_set_gpio_read
 *
 * This function sets the GPIO pin to read mode
 *
 * Parameters: GPIO_TypeDef *port - GPIO port
 *             uint16_t pin - GPIO pin
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_set_gpio_read(GPIO_TypeDef *port, uint16_t pin) {
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_set_gpio_write
 *
 * This function sets the GPIO pin to write mode
 *
 * Parameters: GPIO_TypeDef *port - GPIO port
 *             uint16_t pin - GPIO pin
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_set_gpio_write(GPIO_TypeDef *port, uint16_t pin) {
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_set_prg_data_mode
 *
 * This function sets the PRG data bus mode to either read or write mode
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint8_t mode - data bus mode (0: read, 1: write)
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_set_prg_data_mode(data_bus_config_t *config, uint8_t mode) {

    uint32_t reg_a = GPIOA->MODER;
    uint32_t reg_c = GPIOC->MODER;
    uint32_t type_a = GPIOA->OTYPER;
    uint32_t type_c = GPIOC->OTYPER;
    uint32_t pull_a = GPIOA->PUPDR;
    uint32_t pull_c = GPIOC->PUPDR;

    pull_a &= ~0x0000FFFF; // Clear PA0-PA4 (no pull)
    pull_c &= ~0x0000000F; // Clear PC0-PC2 (no pull)
    GPIOA->PUPDR = pull_a;
    GPIOC->PUPDR = pull_c;

    if (mode == 0) { // Configure Data bus for read mode
        // Bare Metal GPIO Speed Optimization
        reg_a &= ~0x000003FF; // Set PA0-PA4 to input (00)
        reg_c &= ~0x0000003F; // Set PC0-PC2 to input (00)
        GPIOA->MODER = reg_a;
        GPIOC->MODER = reg_c;

        type_a &= ~0x0000001F; // Set PA0-PA4 to push-pull
        type_c &= ~0x00000007; // Set PC0-PC2 to push-pull
        GPIOA->OTYPER = type_a;
        GPIOC->OTYPER = type_c;

//        for (uint8_t i = 0; i < 8; i++) {
//            data_bus_set_gpio_read(prg_data_ports[i], prg_data_pins[i]);
//        }
    } else {
        // Bare Metal GPIO Speed Optimization
        reg_a &= ~0x000003FF; // Clear PA0-PA4
        reg_a |= 0x00000155;  // Set PA0-PA4 to output (01)
        GPIOA->MODER = reg_a;

        reg_c &= ~0x0000003F; // Clear PC0-PC2
        reg_c |= 0x00000015;  // Set PC0-PC2 to output (01)
        GPIOC->MODER = reg_c;

        type_a &= ~0x0000001F; // Set PA0-PA4 to push-pull
        type_c &= ~0x00000007; // Set PC0-PC2 to push-pull
        GPIOA->OTYPER = type_a;
        GPIOC->OTYPER = type_c;


//        for (uint8_t i = 0; i < 8; i++) {
//            data_bus_set_gpio_write(prg_data_ports[i], prg_data_pins[i]);
//        }
    }
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_set_chr_data_mode
 *
 * This function sets the CHR data bus mode to either read or write mode
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint8_t mode - data bus mode (0: read, 1: write)
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_set_chr_data_mode(data_bus_config_t *config, uint8_t mode) {
    if (mode == 0) { // Configure Data bus for read mode
        for (uint8_t i = 0; i < 8; i++) {
            data_bus_set_gpio_read(chr_data_ports[i], chr_data_pins[i]);
        }
    } else {
        for (uint8_t i = 0; i < 8; i++) {
            data_bus_set_gpio_write(chr_data_ports[i], chr_data_pins[i]);
        }
    }
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_write_prg_data
 *
 * This function write the PRG data bus
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint8_t data - data to set on the bus
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_write_prg_data(data_bus_config_t *config, uint8_t data) {

    uint32_t reg_a = 0;
    uint32_t reg_c = 0;
    reg_a = GPIOA->ODR;
    reg_c = GPIOC->ODR;

    // Clear relevant bits
    reg_a &= ~0x001F; // Clear PA0-PA4
    reg_c &= ~0x0007; // Clear PC0-PC2

    // Set new data bits
    reg_a |= (reverse_bits5[data & 0x1F]); // Set PA0-PA4
    reg_c |= (reverse_bits3[(data >> 5) & 0x07]); // Set PC0-PC2

    GPIOA->ODR = reg_a;
    GPIOC->ODR = reg_c;

//    for (uint8_t i = 0; i < 8; i++) {
//        HAL_GPIO_WritePin(prg_data_ports[i], prg_data_pins[i],
//                (data & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//    }
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_read_prg_data
 *
 * This function reads the PRG data bus
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 * Return: uint8_t - data on the bus
 *---------------------------------------------------------------------------*/
uint8_t data_bus_read_prg_data(data_bus_config_t *config) {
    uint8_t data = 0;
//    for (uint8_t i = 0; i < 8; i++) {
//        data |= HAL_GPIO_ReadPin(prg_data_ports[i], prg_data_pins[i]) << i;
//    }
//    data |= HAL_GPIO_ReadPin(CPU_D7_GPIO_Port, CPU_D7_Pin) << 7;
//    data |= HAL_GPIO_ReadPin(CPU_D6_GPIO_Port, CPU_D6_Pin) << 6;
//    data |= HAL_GPIO_ReadPin(CPU_D5_GPIO_Port, CPU_D5_Pin) << 5;
//    data |= HAL_GPIO_ReadPin(CPU_D4_GPIO_Port, CPU_D4_Pin) << 4;
//    data |= HAL_GPIO_ReadPin(CPU_D3_GPIO_Port, CPU_D3_Pin) << 3;
//    data |= HAL_GPIO_ReadPin(CPU_D2_GPIO_Port, CPU_D2_Pin) << 2;
//    data |= HAL_GPIO_ReadPin(CPU_D1_GPIO_Port, CPU_D1_Pin) << 1;
//    data |= HAL_GPIO_ReadPin(CPU_D0_GPIO_Port, CPU_D0_Pin);

    uint8_t a = 0;
    a = GPIOC->IDR & 0x0007;
    data = reverse_bits3[a];

    a = GPIOA->IDR & 0x001F;
    data |= reverse_bits5[a];

//    data |= ((GPIOC->IDR & 0x0001) ? 1 : 0) << 7;
//    data |= ((GPIOC->IDR & 0x0002) ? 1 : 0) << 6;
//    data |= ((GPIOC->IDR & 0x0004) ? 1 : 0) << 5;
//    data |= ((GPIOA->IDR & 0x0001) ? 1 : 0) << 4;
//    data |= ((GPIOA->IDR & 0x0002) ? 1 : 0) << 3;
//    data |= ((GPIOA->IDR & 0x0004) ? 1 : 0) << 2;
//    data |= ((GPIOA->IDR & 0x0008) ? 1 : 0) << 1;
//    data |= ((GPIOA->IDR & 0x0010) ? 1 : 0);

    return data;
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_write_chr_data
 *
 * This function writes the CHR data bus
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint8_t data - data to set on the bus
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_write_chr_data(data_bus_config_t *config, uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(chr_data_ports[i], chr_data_pins[i],
                (data & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_read_chr_data
 *
 * This function read the CHR data bus
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 * Return: uint8_t - data on the bus
 *---------------------------------------------------------------------------*/
uint8_t data_bus_read_chr_data(data_bus_config_t *config) {
    volatile uint8_t data = 0;
    volatile uint32_t a = 0;

//    for (uint8_t i = 0; i < 8; i++) {
//        data |= HAL_GPIO_ReadPin(chr_data_ports[i], chr_data_pins[i]) << i;
//    }

    // Bare metal for performance improvement

    a = GPIOC->IDR & 0x03C0; // PPU D3-D6 (PC9-PC6)
    data |= (reverse_bits5[a >> 6] << 2) & 0xFF;

    a = GPIOA->IDR & 0x0700; // PPU D0-D2 (PA10-PA8)
    data |= (reverse_bits5[a >> 8] >> 2) & 0xFF;

    data |= (((GPIOB->IDR & 0x8000) ? 1 : 0) << 7) & 0xFF; // PPU D7 (PB15)

    return data;
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_configure_nes_bus
 *
 * This function configures the NES bus for the cartridge
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_configure_nes_bus(data_bus_config_t *config) {
    data_bus_set_prg_data_mode(config, CART_READ_MODE);
    data_bus_set_chr_data_mode(config, CART_READ_MODE);
    data_bus_set_gpio_read(CIRAM_A10_GPIO_Port, CIRAM_A10_Pin);
    data_bus_set_gpio_read(CIRAM_CE_GPIO_Port, CIRAM_CE_Pin);
    data_bus_set_gpio_read(CPU_IRQ_GPIO_Port, CPU_IRQ_Pin);
    data_bus_set_gpio_write(PPU_RD_GPIO_Port, PPU_RD_Pin);
    data_bus_set_gpio_write(PPU_WR_GPIO_Port, PPU_WR_Pin);
    data_bus_set_gpio_write(ROMSEL_GPIO_Port, ROMSEL_Pin);
    data_bus_set_gpio_write(CPU_RW_GPIO_Port, CPU_RW_Pin);
    data_bus_set_gpio_write(M2_GPIO_Port, M2_Pin);

    // Set the CPU address bus to 0
    data_bus_set_prg_address(config, 0x8000);
    // Set the PPU address bus to 0
    data_bus_set_chr_address(config, 0x2000);
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_configure_all_output
 *
 * This function configures all the GPIO pins to output mode
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_configure_all_output(data_bus_config_t *config) {
    data_bus_set_prg_data_mode(config, CART_WRITE_MODE);
    data_bus_set_chr_data_mode(config, CART_WRITE_MODE);
    data_bus_set_gpio_write(CIRAM_A10_GPIO_Port, CIRAM_A10_Pin);
    data_bus_set_gpio_write(CIRAM_CE_GPIO_Port, CIRAM_CE_Pin);
    data_bus_set_gpio_write(CPU_IRQ_GPIO_Port, CPU_IRQ_Pin);
    data_bus_set_gpio_write(PPU_RD_GPIO_Port, PPU_RD_Pin);
    data_bus_set_gpio_write(PPU_WR_GPIO_Port, PPU_WR_Pin);
    data_bus_set_gpio_write(ROMSEL_GPIO_Port, ROMSEL_Pin);
    data_bus_set_gpio_write(CPU_RW_GPIO_Port, CPU_RW_Pin);
    data_bus_set_gpio_write(M2_GPIO_Port, M2_Pin);

    // Set the CPU address bus to 0
    data_bus_set_prg_address(config, 0);
    // Set the PPU address bus to 0
    data_bus_set_chr_address(config, 0);
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_configure_all_input
 *
 * This function configures all the GPIO pins to input mode except ADDR bus
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_configure_all_input(data_bus_config_t *config) {
    data_bus_set_prg_data_mode(config, CART_READ_MODE);
    data_bus_set_chr_data_mode(config, CART_READ_MODE);
    data_bus_set_gpio_read(CIRAM_A10_GPIO_Port, CIRAM_A10_Pin);
    data_bus_set_gpio_read(CIRAM_CE_GPIO_Port, CIRAM_CE_Pin);
    data_bus_set_gpio_read(CPU_IRQ_GPIO_Port, CPU_IRQ_Pin);
    data_bus_set_gpio_read(PPU_RD_GPIO_Port, PPU_RD_Pin);
    data_bus_set_gpio_read(PPU_WR_GPIO_Port, PPU_WR_Pin);
    data_bus_set_gpio_read(ROMSEL_GPIO_Port, ROMSEL_Pin);
    data_bus_set_gpio_read(CPU_RW_GPIO_Port, CPU_RW_Pin);
    data_bus_set_gpio_read(M2_GPIO_Port, M2_Pin);
}

void data_bus_get_prg_data(data_bus_config_t *config, uint16_t address, uint8_t *data) {
    data_bus_m2(config, DATA_BUS_CLOCK_LOW); // Set M2 to set address
    data_bus_set_prg_address(config, address); // Set CPU address
    data_bus_m2(config, DATA_BUS_CLOCK_HIGH); // Set M2 high to stabilize
    data_bus_set_romsel(config, address); // Set ROMSEL low to enable ROM
//    _delay_us(1); // Delay to stabilize address and for ROM to respond
    *data = data_bus_read_prg_data(config);
}

void data_bus_get_chr_data(data_bus_config_t *config, uint16_t address, uint8_t *data) {
    data_bus_m2(config, DATA_BUS_CLOCK_HIGH); // Set M2 to high
    data_bus_romsel(config, DATA_BUS_ROMSEL_INACTIVE); // Set ROMSEL high to disable ROM
    data_bus_set_chr_address(config, address); // Set CPU address
    data_bus_chr_RD(config, DATA_BUS_PPU_RD_ACTIVE); // Set RD low to read data
    *data = data_bus_read_chr_data(config);
    data_bus_chr_RD(config, DATA_BUS_PPU_RD_INACTIVE); // Set RD high to disable read mode
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_read_prg
 *
 * This function reads the PRG data from the cartridge
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *            uint16_t address - address to read from
 *
 * Return: uint8_t - data read from the cartridge
 *---------------------------------------------------------------------------*/
uint8_t data_bus_read_prg(data_bus_config_t *config, uint16_t address) {
    uint8_t data = 0;
    data_bus_set_prg_data_mode(config, CART_READ_MODE);

    data_bus_m2(config, DATA_BUS_CLOCK_LOW); // Set M2 to low to change CPU_RW mode and to set address
    data_bus_set_prg_rw(config, CART_READ_MODE); // Set CPU_RW high to read data
    data_bus_set_prg_address(config, address); // Set CPU address
    data_bus_m2(config, DATA_BUS_CLOCK_HIGH); // Set M2 high to stabilize
    data_bus_set_romsel(config, address); // Set ROMSEL low to enable ROM
//    _delay_us(1); // Delay to stabilize address and for ROM to respond
    data_bus_m2(config, DATA_BUS_CLOCK_LOW); // Set M2 low to read data
    data_bus_get_prg_data(config, address, &data);

    return data;
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_read_prg_block
 *
 * This function reads the PRG data block from the cartridge
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint16_t address - address to read from
 *             uint8_t *data - data buffer to store the data
 *             uint16_t size - size of the data block
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_read_prg_block(data_bus_config_t *config, uint16_t address, uint8_t *data, uint16_t size) {
    data_bus_set_prg_data_mode(config, CART_READ_MODE);

    data_bus_m2(config, DATA_BUS_CLOCK_LOW); // Set M2 to low to change CPU_RW mode and to set address
    data_bus_set_prg_rw(config, CART_READ_MODE); // Set CPU_RW high to read data
//    _delay_us(1); // Delay to stabilize address and for ROM to respond
    data_bus_m2(config, DATA_BUS_CLOCK_LOW); // Set M2 low to read data
    for (uint16_t i = 0; i < size; i++) {
        data_bus_get_prg_data(config, address + i, &data[i]);
    }
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_read_chr_block
 *
 * This function reads the CHR data block from the cartridge
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint16_t address - address to read from
 *             uint8_t *data - data buffer to store the data
 *             uint16_t size - size of the data block
 *
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_read_chr_block(data_bus_config_t *config, uint16_t address, uint8_t *data, uint16_t size) {

    data_bus_set_chr_data_mode(config, CART_READ_MODE);

    data_bus_m2(config, DATA_BUS_CLOCK_HIGH); // Set M2 to high
    data_bus_romsel(config, DATA_BUS_ROMSEL_INACTIVE); // Set ROMSEL high to disable ROM

    for (uint16_t i = 0; i < size; i++) {
        data_bus_get_chr_data(config, address + i, &data[i]);
    }
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_write_prg
 *
 * This function writes the PRG data to the cartridge
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint16_t address - address to write to
 *             uint8_t data - data to write
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_write_prg(data_bus_config_t *config, uint16_t address, uint8_t data) {

    data_bus_m2(config, DATA_BUS_CLOCK_LOW); // Set M2 to low to change CPU_RW mode and to set address
    data_bus_romsel(config, DATA_BUS_ROMSEL_INACTIVE); // Set ROMSEL high to disable ROM
    data_bus_set_prg_data_mode(config, CART_WRITE_MODE);
    data_bus_set_prg_rw(config, CART_WRITE_MODE); // Set CPU_RW low to write data
    data_bus_write_prg_data(config, data); // Write data to the bus

    data_bus_set_prg_address(config, address); // Set CPU address
//    _delay_us(1); // Delay to stabilize address and for ROM to respond)
    data_bus_m2(config, DATA_BUS_CLOCK_HIGH); // Set M2 high to stabilize
    data_bus_set_romsel(config, address); // Set ROMSEL low to enable ROM
    _delay_us(1); // Delay to stabilize address and for ROM to respond

    data_bus_m2(config, DATA_BUS_CLOCK_LOW); // Set M2 low to write data
    _delay_us(1); // Delay to stabilize address and for ROM to respond
    data_bus_romsel(config, DATA_BUS_ROMSEL_INACTIVE); // Set ROMSEL high to disable ROM

    data_bus_set_prg_rw(config, CART_READ_MODE); // Set CPU_RW high to read data
    data_bus_set_prg_data_mode(config, CART_READ_MODE);
    data_bus_set_prg_address(config, 0); // Clear PRG address

    data_bus_m2(config, DATA_BUS_CLOCK_HIGH); // Set M2 high to stabilize

//    data_bus_write_prg_data(config, data); // Write data to the bus
//    data_bus_set_prg_address(config, address); // Set CPU address
//    data_bus_set_prg_data_mode(config, CART_WRITE_MODE);
//    data_bus_romsel(config, DATA_BUS_ROMSEL_ACTIVE); // Set ROMSEL low to enable ROM
//    data_bus_m2(config, DATA_BUS_CLOCK_HIGH); // Set M2 high to stabilize)
//    data_bus_set_prg_rw(config, CART_WRITE_MODE); // Set CPU_RW low to write data
//    _delay_us(1); // Delay to stabilize address and for ROM to respond
//    data_bus_m2(config, DATA_BUS_CLOCK_LOW); // Set M2 low to write data
//    data_bus_set_prg_rw(config, CART_READ_MODE); // Set CPU_RW high to read data
//    data_bus_set_prg_data_mode(config, CART_READ_MODE);
//    data_bus_romsel(config, DATA_BUS_ROMSEL_INACTIVE); // Set ROMSEL high to disable ROM
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_write_reg
 *
 * This function writes the data to the MMC1 register
 * This is a fix for MMC1 RAM corruption
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint16_t address - address to write to
 *             uint8_t data - data to write
 *
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_write_reg(data_bus_config_t *config, uint16_t address, uint8_t data) {
    data_bus_m2(config, DATA_BUS_CLOCK_LOW); // Set M2 to low to change CPU_RW mode and to set address
    data_bus_romsel(config, DATA_BUS_ROMSEL_INACTIVE); // Set ROMSEL high to disable ROM
    data_bus_set_prg_data_mode(config, CART_WRITE_MODE);
    data_bus_set_prg_rw(config, CART_WRITE_MODE); // Set CPU_RW low to write data
    data_bus_write_prg_data(config, data); // Write data to the bus

    data_bus_set_prg_address(config, address); // Set CPU address

    // Direct pin to prevent RAM corruption
    // Difference between M2 low and ROMSEL high must be around 33 ns
    // If time is greater than 33 ns then write to RAM will be corrupted
    // Bare Metal

    uint32_t gpioc_odr = GPIOC->ODR & ~(1 << 4);
    uint32_t gpiob_odr = GPIOB->ODR | (1 << 0);
    uint32_t gpioc_set = GPIOC->ODR | (1 << 4);
    uint32_t gpiob_reset = GPIOB->ODR & ~(1 << 0);

    GPIOC->ODR = gpioc_odr; // Set ROMSEL low
    GPIOB->ODR = gpiob_odr; // Set M2 high

    GPIOC->ODR = gpioc_set; // Set ROMSEL high
    GPIOB->ODR = gpiob_reset; // Set M2 low

    _delay_us(1); // Delay to stabilize address and for ROM to respond

    data_bus_set_prg_rw(config, CART_READ_MODE); // Set CPU_RW high to read data
    data_bus_set_prg_data_mode(config, CART_READ_MODE);
    data_bus_set_prg_address(config, 0); // Clear PRG address

    data_bus_m2(config, DATA_BUS_CLOCK_HIGH); // Set M2 high to stabilize
}

/*-----------------------------------------------------------------------------
 * Function: data_bus_test_output_bus
 *
 * This function tests the output bus for the cartridge
 *
 * Parameters: data_bus_config_t *config - cartridge configuration
 *             uint8_t position - position to test
 * Return: None
 *---------------------------------------------------------------------------*/
void data_bus_test_output_bus(data_bus_config_t *config, uint8_t position) {
    if (position <= 14) {
        data_bus_set_prg_address(config, 1 << position);
        data_bus_set_chr_address(config, 0);
    } else if (position > 22 && position <= 37) {
        if (position == 23) {
            data_bus_write_prg_data(config, 0);  // clear cpu data
        }
        data_bus_set_chr_address(config, 1 << (position - 23));
        data_bus_set_prg_address(config, 0);
    } else if (position > 14 && position <= 22) {
        data_bus_write_prg_data(config, 1 << (position - 15));
        data_bus_set_prg_address(config, 0);
        data_bus_set_chr_address(config, 0);
    } else if (position > 37 && position <= 45) {
        data_bus_write_chr_data(config, 1 << (position - 38));
        data_bus_set_prg_address(config, 0);
        data_bus_set_chr_address(config, 0);
    } else {
        data_bus_set_prg_address(config, 0);
        data_bus_set_chr_address(config, 0);
        switch (position) {
        case 46:
            data_bus_write_chr_data(config, 0);  // clear ppu data
            HAL_GPIO_WritePin(PPU_RD_GPIO_Port, PPU_RD_Pin, GPIO_PIN_SET);
            break;
        case 47:
            HAL_GPIO_WritePin(PPU_RD_GPIO_Port, PPU_RD_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(PPU_WR_GPIO_Port, PPU_WR_Pin, GPIO_PIN_SET);
            break;
        case 48:
            HAL_GPIO_WritePin(PPU_WR_GPIO_Port, PPU_WR_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(CPU_RW_GPIO_Port, CPU_RW_Pin, GPIO_PIN_SET);
            break;
        case 49:
            HAL_GPIO_WritePin(CPU_RW_GPIO_Port, CPU_RW_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(CPU_IRQ_GPIO_Port, CPU_IRQ_Pin, GPIO_PIN_SET);
            break;
        case 50:
            HAL_GPIO_WritePin(CPU_IRQ_GPIO_Port, CPU_IRQ_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(ROMSEL_GPIO_Port, ROMSEL_Pin, GPIO_PIN_SET);
            break;
        case 51:
            HAL_GPIO_WritePin(ROMSEL_GPIO_Port, ROMSEL_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(M2_GPIO_Port, M2_Pin, GPIO_PIN_SET);
            break;
        case 52:
            HAL_GPIO_WritePin(M2_GPIO_Port, M2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(CIRAM_CE_GPIO_Port, CIRAM_CE_Pin, GPIO_PIN_SET);
            break;
        case 53:
            HAL_GPIO_WritePin(CIRAM_CE_GPIO_Port, CIRAM_CE_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(CIRAM_A10_GPIO_Port, CIRAM_A10_Pin, GPIO_PIN_SET);
            break;
        case 54:
            HAL_GPIO_WritePin(CIRAM_A10_GPIO_Port, CIRAM_A10_Pin, GPIO_PIN_RESET);
            break;
        default:
            __asm("nop");
            break;
        }
    }
}

