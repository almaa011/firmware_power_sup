#pragma once

#include "stm32l4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initialise PWM hardware that drives LED2 (PA5 on the Nucleo-L476RG).
// Must be called after HAL_Init and SystemClock_Config so the timer reads
// the correct clock tree configuration.
void LED2_PWM_Init(void);

// Update LED2 brightness. Pass a value in the range [0.0f, 1.0f], where 0
// turns the LED off and 1 drives it at full duty cycle. Values outside the
// range are clamped to keep the timer in a valid state.
void LED2_SetBrightness(float brightness);

#ifdef __cplusplus
}
#endif

