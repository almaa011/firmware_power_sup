#pragma once

#include "hal.h"

void timer_handler_callback(TIM_HandleTypeDef* htim);
extern "C" void TIM2_IRQHandler(void);
extern "C" void TIM7_DAC_IRQHandler(void);
extern "C" void FDCAN1_IT0_IRQHandler(void);
extern "C" void FDCAN1_IT1_IRQHandler(void);
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hcan, uint32_t RxFifo0ITs);
void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef* hcan,
                                        uint32_t BufferIndexes);
