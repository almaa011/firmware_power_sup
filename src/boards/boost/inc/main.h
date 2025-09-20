/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

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

/* USER CODE BEGIN Private defines */
#define BOOST_I2C1_INSTANCE I2C1
#define BOOST_I2C1_TIMING 0x00100D14U
#define BOOST_I2C1_OWN_ADDRESS1 0U
#define BOOST_I2C1_ADDRESSING_MODE I2C_ADDRESSINGMODE_7BIT
#define BOOST_I2C1_DUAL_ADDRESS_MODE I2C_DUALADDRESS_DISABLE
#define BOOST_I2C1_OWN_ADDRESS2 0U
#define BOOST_I2C1_OWN_ADDRESS2_MASKS I2C_OA2_NOMASK
#define BOOST_I2C1_GENERAL_CALL_MODE I2C_GENERALCALL_DISABLE
#define BOOST_I2C1_NO_STRETCH_MODE I2C_NOSTRETCH_DISABLE

#define BOOST_SPI1_INSTANCE SPI1
#define BOOST_SPI1_MODE SPI_MODE_MASTER
#define BOOST_SPI1_DIRECTION SPI_DIRECTION_2LINES
#define BOOST_SPI1_DATA_SIZE SPI_DATASIZE_4BIT
#define BOOST_SPI1_CLK_POLARITY SPI_POLARITY_LOW
#define BOOST_SPI1_CLK_PHASE SPI_PHASE_1EDGE
#define BOOST_SPI1_NSS SPI_NSS_SOFT
#define BOOST_SPI1_BAUDRATE_PRESCALER SPI_BAUDRATEPRESCALER_2
#define BOOST_SPI1_FIRST_BIT SPI_FIRSTBIT_MSB
#define BOOST_SPI1_TI_MODE SPI_TIMODE_DISABLE
#define BOOST_SPI1_CRC_CALCULATION SPI_CRCCALCULATION_DISABLE
#define BOOST_SPI1_CRC_POLYNOMIAL 7U
#define BOOST_SPI1_CRC_LENGTH SPI_CRC_LENGTH_DATASIZE
#define BOOST_SPI1_NSSP_MODE SPI_NSS_PULSE_ENABLE

#define BOOST_USB_OTG_FS_INSTANCE USB_OTG_FS
#define BOOST_USB_OTG_FS_HOST_CHANNELS 8U
#define BOOST_USB_OTG_FS_SPEED HCD_SPEED_FULL
#define BOOST_USB_OTG_FS_DMA_ENABLE DISABLE
#define BOOST_USB_OTG_FS_PHY_INTERFACE HCD_PHY_EMBEDDED
#define BOOST_USB_OTG_FS_SOF_ENABLE ENABLE

#define BOOST_VOLTAGE_SCALING PWR_REGULATOR_VOLTAGE_SCALE1
#define BOOST_RCC_OSCILLATOR_TYPE RCC_OSCILLATORTYPE_MSI
#define BOOST_RCC_MSI_STATE RCC_MSI_ON
#define BOOST_RCC_MSI_CALIBRATION 0U
#define BOOST_RCC_MSI_CLOCK_RANGE RCC_MSIRANGE_6
#define BOOST_RCC_PLL_STATE RCC_PLL_NONE
#define BOOST_RCC_CLOCK_TYPE (RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2)
#define BOOST_RCC_SYSCLK_SOURCE RCC_SYSCLKSOURCE_MSI
#define BOOST_RCC_AHBCLK_DIVIDER RCC_SYSCLK_DIV1
#define BOOST_RCC_APB1CLK_DIVIDER RCC_HCLK_DIV1
#define BOOST_RCC_APB2CLK_DIVIDER RCC_HCLK_DIV1
#define BOOST_FLASH_LATENCY FLASH_LATENCY_0
#define BOOST_I2C1_ANALOG_FILTER I2C_ANALOGFILTER_ENABLE
#define BOOST_I2C1_DIGITAL_FILTER 0U
#define BOOST_DMA1_ENABLE_CLOCK() do { __HAL_RCC_DMA1_CLK_ENABLE(); } while (0)
#define BOOST_DMA1_CH2_PRIORITY 0U
#define BOOST_DMA1_CH2_SUBPRIORITY 0U
#define BOOST_DMA1_CH3_PRIORITY 0U
#define BOOST_DMA1_CH3_SUBPRIORITY 0U
#define BOOST_GPIO_ENABLE_PORTS() do { __HAL_RCC_GPIOA_CLK_ENABLE(); __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)
#define BOOST_MAIN_LOOP_DELAY_MS 5U
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
