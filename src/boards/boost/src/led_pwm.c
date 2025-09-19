/**
 * @file led_pwm.c
 * @brief PWM driver for Nucleo-L476RG LED2 (PA5 → TIM2_CH1).
 *
 * This code configures TIM2 channel 1 to generate a 1 kHz PWM signal
 * with 1000 steps of resolution (0.1% brightness steps).
 */

#include "led_pwm.h"

#include "main.h"

// ------------------------------------------------------------
// Hardware definitions
// ------------------------------------------------------------

// The built-in Nucleo LED2 lives on PA5 and is connected to TIM2 channel 1.
#define LED2_GPIO_PORT GPIOA
#define LED2_GPIO_PIN GPIO_PIN_5
#define LED2_GPIO_AF GPIO_AF1_TIM2
#define LED2_TIMER TIM2
#define LED2_TIMER_CHANNEL TIM_CHANNEL_1

// ------------------------------------------------------------
// PWM configuration
// ------------------------------------------------------------

// We target a 1 kHz PWM frequency with 1000 steps of resolution.
// -> Timer runs at 1 MHz (1 µs per tick)
// -> Auto-reload value = 999 (counts 0..999)
// -> Period = 1000 µs = 1 kHz frequency
static const uint32_t kDesiredPwmFrequencyHz = 1000U;
static const uint32_t kTimerResolution = 1000U;  // 0..999 duty-cycle steps

// HAL timer handle
static TIM_HandleTypeDef s_ledTimer = {0};

// ------------------------------------------------------------
// Internal helpers
// ------------------------------------------------------------

/**
 * @brief Configure PA5 as alternate function TIM2_CH1.
 */
static void ConfigureLedGpio(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();  // Enable GPIOA clock

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = LED2_GPIO_PIN;
    gpio.Mode = GPIO_MODE_AF_PP;        // Alternate function, push-pull
    gpio.Pull = GPIO_NOPULL;            // No internal pull-up/down
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;  // Fast enough for PWM
    gpio.Alternate = LED2_GPIO_AF;      // Route TIM2_CH1 to PA5
    HAL_GPIO_Init(LED2_GPIO_PORT, &gpio);
}

/**
 * @brief Compute prescaler so TIM2 runs at target clock.
 * TIM2 is on APB1. If APB1 prescaler > 1, timer clock = 2 × PCLK1.
 */
static uint32_t ComputeTimerPrescaler(uint32_t target_timer_clock) {
    // Extract APB1 prescaler bits
    uint32_t apb_prescaler_bits =
        (RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;
    uint32_t apb_divider =
        1U << ((apb_prescaler_bits >= 4U) ? (apb_prescaler_bits - 3U) : 0U);

    // Base APB1 frequency
    uint32_t timer_clock = HAL_RCC_GetPCLK1Freq();

    // Double if APB prescaler != 1
    if (apb_divider != 1U) {
        timer_clock *= 2U;
    }

    // Compute prescaler (integer division)
    uint32_t prescaler = 0U;
    if (target_timer_clock > 0U && timer_clock > target_timer_clock) {
        prescaler = (timer_clock / target_timer_clock) - 1U;
    }
    return prescaler;
}

/**
 * @brief Configure TIM2 channel 1 for PWM on PA5.
 */
static void ConfigureLedTimer(void) {
    __HAL_RCC_TIM2_CLK_ENABLE();  // Enable TIM2 clock

    const uint32_t target_timer_clock =
        kDesiredPwmFrequencyHz * kTimerResolution;  // 1 MHz
    const uint32_t prescaler = ComputeTimerPrescaler(target_timer_clock);

    // Timer base setup
    s_ledTimer.Instance = LED2_TIMER;
    s_ledTimer.Init.Prescaler = prescaler;
    s_ledTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
    s_ledTimer.Init.Period = (kTimerResolution - 1U);  // 999
    s_ledTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    s_ledTimer.Init.RepetitionCounter = 0U;
    s_ledTimer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_PWM_Init(&s_ledTimer) != HAL_OK) {
        Error_Handler();  // Provided by CubeMX template
    }

    // PWM channel setup
    TIM_OC_InitTypeDef config = {0};
    config.OCMode = TIM_OCMODE_PWM1;  // Active when counter < compare
    config.Pulse = 0U;                // Start off (0% duty)
    config.OCPolarity = TIM_OCPOLARITY_HIGH;
    config.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(&s_ledTimer, &config, LED2_TIMER_CHANNEL) !=
        HAL_OK) {
        Error_Handler();
    }
}

// ------------------------------------------------------------
// Public API
// ------------------------------------------------------------

/**
 * @brief Initialize PWM output for LED2.
 */
void LED2_PWM_Init(void) {
    ConfigureLedGpio();   // Setup PA5
    ConfigureLedTimer();  // Setup TIM2 CH1

    // Start PWM generation
    if (HAL_TIM_PWM_Start(&s_ledTimer, LED2_TIMER_CHANNEL) != HAL_OK) {
        Error_Handler();
    }

    // Ensure the LED starts OFF
    LED2_SetBrightness(0.0f);
}

/**
 * @brief Set LED brightness as float [0.0 .. 1.0].
 *
 * @param brightness 0.0 = off, 1.0 = max brightness
 */
void LED2_SetBrightness(float brightness) {
    if (brightness < 0.0f) {
        brightness = 0.0f;
    } else if (brightness > 1.0f) {
        brightness = 1.0f;
    }

    const uint32_t compare =
        (uint32_t)(brightness * (float)(kTimerResolution - 1U));

    __HAL_TIM_SET_COMPARE(&s_ledTimer, LED2_TIMER_CHANNEL, compare);
}
