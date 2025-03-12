/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #include <stdio.h>
 
 #include "HalGpio.h"
 #include "UtilGen.h"
 #include "SrvScheduler.h"

void tastBlinkLED(void)
{
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

