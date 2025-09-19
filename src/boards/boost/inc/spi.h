#pragma once

#include "stm32l4xx_hal.h"

extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern SPI_HandleTypeDef hspi1;
void SPI_Init(void);
void send_bytes_SPI(uint8_t *send, uint32_t num);
void send_receive_bytes_SPI(uint8_t *send, uint8_t *receive, uint32_t num);
