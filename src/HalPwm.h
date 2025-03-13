#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <nrfx_pwm.h>

#define HAL_PWM_N_OF_CONNECTED_CHANNELS 4

void PWM_Init(void);
void PWM_SetDutyCycle(uint8_t channel, uint8_t duty_cycle);

#endif // PWM_H
