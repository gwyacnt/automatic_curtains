#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <nrfx_pwm.h>


typedef enum
{
    HAL_PWM_CHANNEL_LED3_DEVKIT = 0,
    HAL_PWM_CHANNEL_MOTOR_R_PWM = 1,                                                 
    HAL_PWM_CHANNEL_MOTOR_L_PWM = 2,                                                 
    HAL_PWM_CHANNEL_LED4_DEVKIT = 3,  
    HAL_PWM_N_OF_CONNECTED_CHANNELS
} HAL_PWM_CHANNEL_t;

void PWM_Init(void);
void PWM_SetDutyCycle(HAL_PWM_CHANNEL_t channel, uint8_t duty_cycle);

#endif // PWM_H
