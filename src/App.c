

#include <stdio.h>
#include "HalGpio.h"
#include "UtilGen.h"
#include "SrvScheduler.h"
#include "HalPwm.h"
#include "HalTim.h"
#include "AppEncoder.h"
#include <zephyr/sys/printk.h>

void tastBlinkLED(void)
{
    // static uint64_t last_time_ms = 0;  // Store last execution time
    static uint8_t duty_cycle[4];
    // uint64_t current_time_ms = k_uptime_get();  // Get current system time
    // if (last_time_ms != 0) 
    // {
    //     uint64_t elapsed = current_time_ms - last_time_ms;
    //     printk(" - Elapsed time since last execution: %llu ms\n", elapsed);
    // }
    // last_time_ms = current_time_ms;  // Update last execution time

    HalGpio_TogglePin (PIN_LED1_DEVKIT);

    duty_cycle[0] = (duty_cycle[0]+5)%100;
    duty_cycle[3] = (duty_cycle[3]+20)%100;
    PWM_SetDutyCycle(0, duty_cycle[0]);
    PWM_SetDutyCycle(3, duty_cycle[3]);
}


void AppProcess(void)
{
     HalGpio_Init();
     PWM_Init();
     SrvScheduler_Init();
     SrvScheduler_AddTask(tastBlinkLED, 1000);
     SrvScheduler_Start();
     AppEncoder_Init();
 
     while (1) 
     {
        // if(HalGpio_ReadPin(PIN_ENC_A) == 1)
        // {
        //     printk("H\n");
        // }
        // else if(HalGpio_ReadPin(PIN_ENC_A) == 0)
        // {
        //     printk("L\n");
        // }
        //  ret = HalGpio_TogglePin(PIN_LED1_DEVKIT);
        // HalGpio_WritePin (PIN_LED1_DEVKIT, 0);
        // UtilGen_delay_ms(SLEEP_TIME_MS);
        // HalGpio_WritePin (PIN_LED1_DEVKIT, 1);
        // UtilGen_delay_ms(SLEEP_TIME_MS);
     }
}