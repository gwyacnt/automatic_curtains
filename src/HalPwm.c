#include "HalPwm.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(PWM, LOG_LEVEL_INF);


#define PWM_NODE NRF_PWM0
#define PWM_CHANNEL 0
#define PWM_PERIOD_USEC 20000  

static const struct device *pwm_dev;

void PWM_Init(void) 
{
    pwm_dev = DEVICE_DT_GET(DT_NODELABEL(pwm0));
    if (!device_is_ready(pwm_dev))
     {
        LOG_ERR("PWM device not ready!");
        return;
    }
    LOG_INF("PWM Initialized");
}

void PWM_SetDutyCycle(uint8_t duty_cycle) 
{
    if (!device_is_ready(pwm_dev)) return;

    uint32_t pulse_width = (duty_cycle * PWM_PERIOD_USEC) / 100;
    // pwm_set_dt(&pwm_dev, PWM_PERIOD_USEC, pulse_width);
    pwm_set(pwm_dev, 0, PWM_PERIOD_USEC, pulse_width, 0);
    LOG_INF("PWM Duty Cycle: %d%%", duty_cycle);
}
