#include "pwm_driver.h"

#include "light.h"

namespace umnsvp {
namespace lights {
/* Timer handler declaration */
TIM_HandleTypeDef TimHandle;
// Register value of the period
constexpr uint32_t PERIOD_VALUE = (uint32_t)(8000 - 1); /* Period Value  */

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sConfig;

/*counter pre scalar value*/
uint32_t uhPrescalerValue = 0;

/**
 * @brief Sets the brightness of the lights as a percentage of their maximum
 *        value. Channel 2 for light 0, channel 1 for light 1.
 *        Cases for other lights included but unused.
 *        Cases go in order from light 0 to light 5
 *
 *        Note: Depending on the lights used, brightness adjustment may be
 *              somewhat steppy (changes in brightness less obvious in certain
 *              ranges of brightness values)
 *
 * @param brightness_percentage The percentage of max to set the brightness to
 */
void pwmDriver::set_light_brightness(float brightness_percentage) {
    // Correct for bad input into the function
    if (brightness_percentage > 1) {
        brightness_percentage = 1;
    }

    // Using a less than bc float
    // Checks all light objects from light0 to light5
    if (brightness_percentage < 0.01) {
        if (pin == PIN_LIGHT_0 && port == PORT_LIGHT_0) {
            __HAL_TIM_SET_COMPARE(&TimHandle, TIM_CHANNEL_1, 1);
        } else if (pin == PIN_LIGHT_1 && port == PORT_LIGHT_1) {
            __HAL_TIM_SET_COMPARE(&TimHandle, TIM_CHANNEL_2, 1);
        }
        // else if (pin == PIN_LIGHT_2 && port == PORT_LIGHT_2) {
        // } else if (pin == PIN_LIGHT_3 && port == PORT_LIGHT_3) {
        // } else if (pin == PIN_LIGHT_4 && port == PORT_LIGHT_4) {
        // } else if (pin == PIN_LIGHT_5 && port == PORT_LIGHT_5) {
        // }

    }

    else {
        uint16_t PWM = PERIOD_VALUE * brightness_percentage;
        if (pin == PIN_LIGHT_0 && port == PORT_LIGHT_0) {
            __HAL_TIM_SET_COMPARE(&TimHandle, TIM_CHANNEL_1, PWM);
        } else if (pin == PIN_LIGHT_1 && port == PORT_LIGHT_1) {
            __HAL_TIM_SET_COMPARE(&TimHandle, TIM_CHANNEL_2, PWM);
        }
        // else if (pin == PIN_LIGHT_2 && port == PORT_LIGHT_2) {
        // } else if (pin == PIN_LIGHT_3 && port == PORT_LIGHT_3) {
        // } else if (pin == PIN_LIGHT_4 && port == PORT_LIGHT_4) {
        // } else if (pin == PIN_LIGHT_5 && port == PORT_LIGHT_5) {
        // }
    }
}

/**
 * @brief TIM MSP Initialization
 *        This function configures the hardware resources used in this example:
 *           - Peripheral's clock enable
 *           - Peripheral's GPIO Configuration
 * @param htim: TIM handle pointer
 * @retval None
 */
void pwmDriver::init_timer_gpio() {
    /* Enable GPIO Channels Clock */
    if (port == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (port == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else if (port == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    } else if (port == GPIOD) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    } else if (port == GPIOE) {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    } else if (port == GPIOF) {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    } else if (port == GPIOG) {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }

    // Pack the pin configuration into a struct.
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

    // Configure the GPIO port with the packed pin configuration.
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void pwmDriver::init_light_brightness(void) {
    if (pin == PIN_LIGHT_0 && port == PORT_LIGHT_0) {
        HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
    } else if (pin == PIN_LIGHT_1 && port == PORT_LIGHT_1) {
        HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
    }
    // else if (pin == PIN_LIGHT_2 && port == PORT_LIGHT_2) {
    // } else if (pin == PIN_LIGHT_3 && port == PORT_LIGHT_3) {
    // } else if (pin == PIN_LIGHT_4 && port == PORT_LIGHT_4) {
    // } else if (pin == PIN_LIGHT_5 && port == PORT_LIGHT_5) {
    // }
}

/**
 * @brief Initializes the lights by setting up pins and the output
 * compare timer
 */
void pwmDriver::init() {
    __HAL_RCC_TIM1_CLK_ENABLE();
    /*##-1- Configure the TIM peripheral
     * #######################################*/
    /* -----------------------------------------------------------------------
    TIM1 Configuration: generate 4 PWM signals with 4 different duty cycles.

    In this example TIM1 input clock (TIM1CLK) is set to APB1 clock (PCLK1),
    since APB1 prescaler is equal to 1.
        TIM1CLK = PCLK1
        PCLK1 = HCLK
        => TIM1CLK = HCLK = SystemCoreClock

    To get TIM1 counter clock at 20 kHz, the prescaler is computed as follows:
        Prescaler = (TIM1CLK / TIM1 counter clock) - 1
        Prescaler = ((SystemCoreClock) /20 MHz) - 1
        Prescalar = (80MHz/20kHz) - 1
        Prescalar = 3999 , which is in range of 1-65535 for prescale values
        so prescalar can also be ARR.
    ARR = 3999;

    ----------------------------------------------------------------------- */

    /* Initialize TIMx peripheral as follows:
        + Prescaler = (SystemCoreClock / 20000) - 1
        + Period = (4000 - 1)
        + ClockDivision = 0
        + Counter direction = Up
    */
    TimHandle.Instance = TIM1;
    TimHandle.Init.Prescaler = 0;
    TimHandle.Init.Period = PERIOD_VALUE;
    TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&TimHandle);
    /*##-2- Configure the PWM channels
     * #########################################*/
    /* Common configuration for all channels */
    sConfig.OCMode = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;
    sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    init_timer_gpio();
    init_light_brightness();
}

/**
 * @brief Turn on pwm signal generation.
 *        Channel 2 for light 0, channel 1 for light 1.
 *        Cases for the other lights are included but unused.
 *        Cases go in order from light 0 to light 5
 */
void pwmDriver::turn_on() {
    if (pin == PIN_LIGHT_0 && port == PORT_LIGHT_0) {
        HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
    } else if (pin == PIN_LIGHT_1 && port == PORT_LIGHT_1) {
        HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);
    }
    // else if (pin == PIN_LIGHT_2 && port == PORT_LIGHT_2) {
    // } else if (pin == PIN_LIGHT_3 && port == PORT_LIGHT_3) {
    // } else if (pin == PIN_LIGHT_4 && port == PORT_LIGHT_4) {
    // } else if (pin == PIN_LIGHT_5 && port == PORT_LIGHT_5) {
    // }
}

/**
 * @brief Turn off pwm signal generation.
 *        Channel 2 for light 0, channel 1 for light 1.
 *        Cases for the other lights are included but unused.
 *        Cases go in order from light 0 to light 5
 */
void pwmDriver::turn_off() {
    if (pin == PIN_LIGHT_0 && port == PORT_LIGHT_0) {
        HAL_TIM_PWM_Stop(&TimHandle, TIM_CHANNEL_1);
    } else if (pin == PIN_LIGHT_1 && port == PORT_LIGHT_1) {
        HAL_TIM_PWM_Stop(&TimHandle, TIM_CHANNEL_2);
    }
    // else if (pin == PIN_LIGHT_2 && port == PORT_LIGHT_2) {
    // } else if (pin == PIN_LIGHT_3 && port == PORT_LIGHT_3) {
    // } else if (pin == PIN_LIGHT_4 && port == PORT_LIGHT_4) {
    // } else if (pin == PIN_LIGHT_5 && port == PORT_LIGHT_5) {
    // }
}

}  // namespace lights
}  // namespace umnsvp
