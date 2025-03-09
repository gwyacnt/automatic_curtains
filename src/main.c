#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include <inttypes.h>  // Required for PRIx64

#include "HalGpio.h"

void app_main(void) 
{
    // Init modules
    HalGpio_Init();

    while (1) 
    {
        // Turn LED ON
        HalGpio_WritePin(PIN_LED_DEVKIT, 1);
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay 500ms

        // Turn LED OFF
        HalGpio_WritePin(PIN_LED_DEVKIT, 0);
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay 500ms
        ESP_LOGI("Main", "Flush");
    }
}
