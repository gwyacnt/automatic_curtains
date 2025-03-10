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
 
 /* The devicetree node identifier for the "led0" alias. */
 #define LED0_NODE DT_ALIAS(led0)
 
 /*
  * A build error on this line means your board is unsupported.
  * See the sample documentation for information on how to fix this.
  */
 static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
 
 int main(void)
 {
     int ret;
     bool led_state = true;

     HalGpio_Init();
 
    //  if (!gpio_is_ready_dt(&led)) {
    //      return 0;
    //  }
 
    //  ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    //  if (ret < 0) {
    //      return 0;
    //  }
 
     while (1) 
     {
        HalGpio_TogglePin(PIN_LED2_DEVKIT);
        HalGpio_TogglePin(PIN_LED3_DEVKIT);
         ret = HalGpio_TogglePin(PIN_LED1_DEVKIT);
         if (ret < 0) 
         {
             return 0;
         }
 
         led_state = !led_state;
         printf("LED state: %s\n", led_state ? "ON" : "OFF");
         k_msleep(SLEEP_TIME_MS);
     }
     return 0;
 }
 
// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

// #include "esp_log.h"
// #include <inttypes.h>  // Required for PRIx64

// #include "HalGpio.h"

// void app_main(void) 
// {
//     // Init modules
//     HalGpio_Init();

//     while (1) 
//     {
//         // Turn LED ON
//         HalGpio_WritePin(PIN_LED_DEVKIT, 1);
//         vTaskDelay(pdMS_TO_TICKS(500)); // Delay 500ms

//         // Turn LED OFF
//         HalGpio_WritePin(PIN_LED_DEVKIT, 0);
//         vTaskDelay(pdMS_TO_TICKS(500)); // Delay 500ms
//         ESP_LOGI("Main", "Flush");
//     }
// }
