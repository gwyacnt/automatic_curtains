/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #include <stdio.h>
 #include <zephyr/kernel.h>
 #include <zephyr/drivers/gpio.h>
 
 #include "HalGpio.h"

 /* 1000 msec = 1 sec */
 #define SLEEP_TIME_MS   3000
 
 int main(void)
 {
     int ret;
     bool led_state = true;

     HalGpio_Init();
 
     while (1) 
     {
        //  ret = HalGpio_TogglePin(PIN_LED1_DEVKIT);
        HalGpio_WritePin (PIN_LED1_DEVKIT, 0);
        k_msleep(SLEEP_TIME_MS);
        HalGpio_WritePin (PIN_LED1_DEVKIT, 1);
        k_msleep(SLEEP_TIME_MS);
     }
     return 0;
 }

