#include "mosfet_pwm.h"

#define MOSFET_PWM_PERIOD_TICKS   (255U)
#define MOSFET_PWM_DEFAULT_COMPARE (125U)
#define MOSFET_PWM_CHANNEL        TIM_CHANNEL_1
#define MOSFET_PWM_MIN_DUTY       (0.0f)
#define MOSFET_PWM_MAX_DUTY       (1.0f)

TIM_HandleTypeDef htim1;

static void MX_TIM1_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = MOSFET_PWM_PERIOD_TICKS;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = MOSFET_PWM_DEFAULT_COMPARE;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, MOSFET_PWM_CHANNEL) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim1);
}

void MOSFET_PWM_Init(void)
{
  MX_TIM1_Init();

  __HAL_TIM_SET_COMPARE(&htim1, MOSFET_PWM_CHANNEL, MOSFET_PWM_DEFAULT_COMPARE);
  if (HAL_TIM_PWM_Start(&htim1, MOSFET_PWM_CHANNEL) != HAL_OK)
  {
    Error_Handler();
  }
}

void MOSFET_PWM_SetDutyCycle(float duty_cycle)
{
  if (duty_cycle < MOSFET_PWM_MIN_DUTY)
  {
    duty_cycle = MOSFET_PWM_MIN_DUTY;
  }
  else if (duty_cycle > MOSFET_PWM_MAX_DUTY)
  {
    duty_cycle = MOSFET_PWM_MAX_DUTY;
  }

  if (htim1.Instance == NULL)
  {
    return;
  }

  uint32_t period = __HAL_TIM_GET_AUTORELOAD(&htim1);
  uint32_t compare = (uint32_t)((duty_cycle * (float)(period + 1U)) + 0.5f);
  if (compare > period)
  {
    compare = period;
  }

  __HAL_TIM_SET_COMPARE(&htim1, MOSFET_PWM_CHANNEL, compare);
}
