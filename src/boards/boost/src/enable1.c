#include "enable1.h"

#include "main.h"
// This helper function configures a GPIO pin (PA2) as a push-pull output and
// sets it high. This will cause PA2 to output 3.3V when the STM32 is powered
// on.

void enable_gpio_pin_always_high(void) {
    // Enable clock for GPIOA
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configure PA2 as push-pull output, no pull-up or pull-down, low speed
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Set PA2 high (logic 1), which will output 3.3V if the board supply
    // is 3.3V
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
}
