/**
 * Minimal HAL configuration for the boost board.
 */
#ifndef STM32L4xx_HAL_CONF_H
#define STM32L4xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

// Enable the handful of HAL modules that our boost firmware actually calls
#define HAL_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED

// Clock source defaults expected by the HAL drivers; tweak here if the board changes
#ifndef HSE_VALUE
#define HSE_VALUE 8000000U
#endif

#ifndef HSE_STARTUP_TIMEOUT
#define HSE_STARTUP_TIMEOUT 100U
#endif

#ifndef MSI_VALUE
#define MSI_VALUE 4000000U
#endif

#ifndef HSI_VALUE
#define HSI_VALUE 16000000U
#endif

#ifndef HSI48_VALUE
#define HSI48_VALUE 48000000U
#endif

#ifndef LSI_VALUE
#define LSI_VALUE 32000U
#endif

#ifndef LSE_VALUE
#define LSE_VALUE 32768U
#endif

#ifndef LSE_STARTUP_TIMEOUT
#define LSE_STARTUP_TIMEOUT 5000U
#endif

// SAI clocks are referenced inside the HAL even if we never touch SAI directly
#ifndef EXTERNAL_SAI1_CLOCK_VALUE
#define EXTERNAL_SAI1_CLOCK_VALUE 2097000U
#endif

#ifndef EXTERNAL_SAI2_CLOCK_VALUE
#define EXTERNAL_SAI2_CLOCK_VALUE 2097000U
#endif

// System-level HAL knobs used by startup code and SysTick configuration
#define VDD_VALUE 3300U
#define TICK_INT_PRIORITY 15U
#define USE_RTOS 0U
#define PREFETCH_ENABLE 0U
#define INSTRUCTION_CACHE_ENABLE 1U
#define DATA_CACHE_ENABLE 1U

// Pull in the minimal set of HAL headers that correspond to the enabled modules
#if defined(HAL_RCC_MODULE_ENABLED)
#include "stm32l4xx_hal_rcc.h"
#include "stm32l4xx_hal_rcc_ex.h"
#endif

#if defined(HAL_FLASH_MODULE_ENABLED)
#include "stm32l4xx_hal_flash.h"
#include "stm32l4xx_hal_flash_ex.h"
#include "stm32l4xx_hal_flash_ramfunc.h"
#endif

#if defined(HAL_GPIO_MODULE_ENABLED)
#include "stm32l4xx_hal_gpio.h"
#endif

#if defined(HAL_PWR_MODULE_ENABLED)
#include "stm32l4xx_hal_pwr.h"
#include "stm32l4xx_hal_pwr_ex.h"
#endif

#if defined(HAL_CORTEX_MODULE_ENABLED)
#include "stm32l4xx_hal_cortex.h"
#endif

#ifdef USE_FULL_ASSERT
#define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
void assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0U)
#endif

#ifdef __cplusplus
}
#endif

#endif
