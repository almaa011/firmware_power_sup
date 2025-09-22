#ifndef MOSFET_PWM_H
#define MOSFET_PWM_H

#include "main.h"

void MOSFET_PWM_Init(void);
void MOSFET_PWM_SetDutyCycle(float duty_cycle);

extern TIM_HandleTypeDef htim1;

#endif  // MOSFET_PWM_H
