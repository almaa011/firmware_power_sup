
#pragma once

#if STM32L476xx
#include "stm32l4xx.h"
#elif STM32F405xx || STM32F469xx
#include "stm32f4xx.h"
#elif STM32G474xx || STM32G473xx
#include "stm32g4xx.h"
#elif STM32G0B1xx
#include "stm32g0xx.h"
#elif STM32L562xx
#include "stm32l5xx.h"
#else
#error "hal.h requires that the microcontroller be defined."
#endif
