#ifndef PWM_H
#define PWM_H

#include <zephyr/drivers/pwm.h>

void PWM_Init(void);
void PWM_SetDutyCycle(uint8_t duty_cycle);

#endif // PWM_H
