#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Define the GPIO pin for the LED
#define LED_PIN GPIO_NUM_2  // Most ESP32 dev kits have an LED on GPIO 2

void app_main(void) {
    // Configure the LED pin as an output
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while (1) 
    {
        // Turn LED ON
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay 500ms

        // Turn LED OFF
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay 500ms
    }
}
