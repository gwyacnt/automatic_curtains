/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #include <stdio.h>
 #define TIMER_ENABLED 1
#define TIMER1_ENABLED 1
 #include "HalGpio.h"
 #include "UtilGen.h"
 #include "SrvScheduler.h"
 #include "HalTim.h"
#include <zephyr/sys/printk.h>

void tastBlinkLED(void)
{
    static uint64_t last_time_ms = 0;  // Store last execution time
    uint64_t current_time_ms = k_uptime_get();  // Get current system time

    if (last_time_ms != 0) {
        uint64_t elapsed = current_time_ms - last_time_ms;
        printk("Elapsed time since last execution: %llu ms\n", elapsed);
    }

    last_time_ms = current_time_ms;  // Update last execution time
    HalGpio_TogglePin (PIN_LED1_DEVKIT);
}
 
 int main(void)
 {
     HalGpio_Init();
     SrvScheduler_Init();
     SrvScheduler_AddTask(tastBlinkLED, 1000);
     SrvScheduler_Start();
 
     while (1) 
     {
        //  ret = HalGpio_TogglePin(PIN_LED1_DEVKIT);
        // HalGpio_WritePin (PIN_LED1_DEVKIT, 0);
        // UtilGen_delay_ms(SLEEP_TIME_MS);
        // HalGpio_WritePin (PIN_LED1_DEVKIT, 1);
        // UtilGen_delay_ms(SLEEP_TIME_MS);
     }
     return 0;
 }

