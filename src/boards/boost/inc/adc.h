#pragma once

#include "stm32l4xx_hal.h"
#include <stdbool.h>

#define READ_REG_OPCODE 0001

HAL_StatusTypeDef read_registers(uint8_t addr, uint8_t num, uint8_t size, uint8_t *results);
HAL_StatusTypeDef write_registers(uint8_t addr, uint8_t num, uint8_t size, const uint8_t *data);
HAL_StatusTypeDef set_mux(uint8_t PSEL);
HAL_StatusTypeDef adc_init(void);
HAL_StatusTypeDef adc_read(uint32_t *result);
