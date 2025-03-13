#include "HalPwm.h"
#include "HalGpio.h"
#include <nrfx_pwm.h>
#include <stdio.h>

#define PWM_INST_IDX 0
#define PWM_PERIOD 16000 //High-Resolution Control (Precise Speed Control)

static nrfx_pwm_t pwm_instance = NRFX_PWM_INSTANCE(PWM_INST_IDX);

// Sequence data (NRF PWM requires sequences for duty cycles)
static nrf_pwm_values_common_t pwm_seq_values[2];
static nrf_pwm_sequence_t pwm_seq = {
    .values.p_common = pwm_seq_values,
    .length = NRF_PWM_VALUES_LENGTH(pwm_seq_values),
    .repeats = 0,
    .end_delay = 0
};


void PWM_Init(void) 
{
    nrfx_err_t status;
    (void)status;
    
    nrfx_pwm_config_t config = {                                                               
        .output_pins   = {                                          
            NRF_PWM_PIN_NOT_CONNECTED,                                                 
            NRF_PWM_PIN_NOT_CONNECTED,                                                 
            NRF_PWM_PIN_NOT_CONNECTED,                                                 
            PIN_LED4_DEVKIT.pin,                                                 
        },                                                          
        .pin_inverted  = {                                          
            false,                                                  
            false,                                                  
            false,                                                  
            false,                                                  
        },                                                          
        .irq_priority  = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,      
        .base_clock    = NRF_PWM_CLK_16MHz, //NRF_PWM_CLK_1MHz,                          
        .count_mode    = NRF_PWM_MODE_UP,                           
        .top_value     = PWM_PERIOD,                                      
        .load_mode     = NRF_PWM_LOAD_COMMON,                       
        .step_mode     = NRF_PWM_STEP_AUTO,                         
        .skip_gpio_cfg = false                                      
    };
    status = nrfx_pwm_init(&pwm_instance, &config, NULL, NULL);
    NRFX_ASSERT(status == NRFX_SUCCESS);
}

// Set PWM Duty Cycle (0-100%)
void PWM_SetDutyCycle(uint8_t channel, uint8_t duty_cycle) 
{
    if (duty_cycle > 100) duty_cycle = 100;

    pwm_seq_values[channel] = (duty_cycle * PWM_PERIOD) / 100;  // Convert to pulse width

    nrfx_pwm_simple_playback(&pwm_instance, &pwm_seq, 1, NRFX_PWM_FLAG_LOOP);
    
    printf("\nPWM Channel %d: Duty Cycle = %d%%", channel, duty_cycle);
}
