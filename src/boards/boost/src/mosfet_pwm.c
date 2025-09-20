#include "mosfet_pwm.h"

// PWM tuning parameters for the boost MOSFET gate drive.
#define MOSFET_PWM_FREQUENCY_HZ        (100000U)  // 100 kHz switching frequency target.
#define MOSFET_PWM_TIMER_INSTANCE     TIM2        // TIM2_CH1 maps to PA0 on the Nucleo-64 D3 header.
#define MOSFET_PWM_TIMER_CHANNEL      TIM_CHANNEL_1
#define MOSFET_PWM_DEFAULT_DUTY       (0.45f)     // 45% duty keeps the converter in a safe startup point.
#define MOSFET_PWM_MIN_DUTY           (0.0f)
#define MOSFET_PWM_MAX_DUTY           (0.98f)     // Leave headroom to guarantee a minimum off-time.

// Internal bookkeeping so duty-cycle updates can translate into timer counts.
static uint32_t s_mosfet_period_ticks = 0U;

// Timer handle is shared with the HAL MSP layer.
TIM_HandleTypeDef htim2;

static uint32_t MOSFET_PWM_GetTimerClockHz(void) {
    // TIM2 sits on APB1; when that bus is prescaled the timer clock runs at 2x the bus clock.
    uint32_t pclk1_hz = HAL_RCC_GetPCLK1Freq();
    uint32_t apb1_prescaler_bits = RCC->CFGR & RCC_CFGR_PPRE1_Msk;
    if (apb1_prescaler_bits >= RCC_CFGR_PPRE1_DIV2) {
        pclk1_hz *= 2U;
    }
    return pclk1_hz;
}

void MOSFET_PWM_SetDutyCycle(float duty_cycle) {
    // Clamp the requested duty cycle to keep the converter within safe bounds.
    if (duty_cycle < MOSFET_PWM_MIN_DUTY) {
        duty_cycle = MOSFET_PWM_MIN_DUTY;
    } else if (duty_cycle > MOSFET_PWM_MAX_DUTY) {
        duty_cycle = MOSFET_PWM_MAX_DUTY;
    }

    if (s_mosfet_period_ticks == 0U) {
        return;  // Timer has not been initialised yet.
    }

    uint32_t compare_ticks = (uint32_t)((duty_cycle * (float)s_mosfet_period_ticks) + 0.5f);  // Convert duty cycle into timer counts.
    if (compare_ticks >= s_mosfet_period_ticks) {  // Prevent a 100% duty cycle that would lock the switch on.
        compare_ticks = s_mosfet_period_ticks - 1U;
    }

    __HAL_TIM_SET_COMPARE(&htim2, MOSFET_PWM_TIMER_CHANNEL, compare_ticks);
}

void MOSFET_PWM_Init(void) {
    // Push the default duty cycle before the timer starts to avoid an initial glitch.
    MOSFET_PWM_SetDutyCycle(MOSFET_PWM_DEFAULT_DUTY);

    // Kick the timer into run mode so the gate waveform appears on PA0 (TIM2_CH1).
    if (HAL_TIM_PWM_Start(&htim2, MOSFET_PWM_TIMER_CHANNEL) != HAL_OK) {
        Error_Handler();
    }
}

void MX_TIM2_Init(void) {
  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  uint32_t timer_clock_hz = MOSFET_PWM_GetTimerClockHz();  // Determine the effective clock feeding TIM2.
  uint64_t ticks = ((uint64_t)timer_clock_hz + (MOSFET_PWM_FREQUENCY_HZ / 2U)) / MOSFET_PWM_FREQUENCY_HZ;  // Round to the nearest whole timer tick.
  if (ticks == 0ULL)  // Protect against an extremely low requested frequency.
  {
    ticks = 1ULL;
  }
  uint32_t period_ticks = (uint32_t)ticks;  // Total counts for one PWM period (ARR + 1).
  if (period_ticks == 0U)  // Ensure ARR never underflows when we subtract 1.
  {
    period_ticks = 1U;
  }

  htim2.Instance = MOSFET_PWM_TIMER_INSTANCE;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = period_ticks - 1U;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  TIM_OC_InitTypeDef sConfigOC = {0};
  uint32_t default_pulse = (uint32_t)((double)period_ticks * (double)MOSFET_PWM_DEFAULT_DUTY + 0.5);  // Preload CCR with the desired gate duty.
  if (default_pulse >= period_ticks)  // Keep at least one timer tick of off-time.
  {
    default_pulse = period_ticks - 1U;
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = default_pulse;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, MOSFET_PWM_TIMER_CHANNEL) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim2);  // Configure the PA0 pin for the PWM alternate function.

  s_mosfet_period_ticks = period_ticks;  // Cache for duty-cycle calculations outside of init.

  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
}
