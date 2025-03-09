/******************************************************************************/

/**
 *  \file    HalGpio.c
 *  \author  https://github.com/gwyacnt/
 *  \brief   GPIO component implementation
 *
 *  \remarks 
 */

 #include <stdint.h>
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include "HalGpio.h"

 #include "driver/gpio.h"
 #include "esp_log.h"
 #include <inttypes.h>  // Required for PRIx64
 /******************************************************************************/
 /* Local defines and types                                                    */
 /******************************************************************************/
 
 /******************************************************************************/
 /* Local function prototypes                                                  */
 /******************************************************************************/
 
 /******************************************************************************/
 /* Local data                                                                 */
 /******************************************************************************/
 
 /******************************************************************************/
 /* Exported data                                                              */
 /******************************************************************************/
 
 /******************************************************************************/
 /* Exported functions                                                         */
 /******************************************************************************/
 /**
   * @brief GPIO Initialization Function
   * @param None
   * @retval None
   */
 void HalGpio_Init(void)
 {
    uint64_t pin_bitmask = 0;
    gpio_config_t out_pins_config;

    // Set output GPIO pins
    // For this project the following pins are output
    // IO_02 = Devkit LED
    // IO_16 = L_EN
    // IO_17 = R_EN
    pin_bitmask |= (PIN_LED_DEVKIT << 1);
    pin_bitmask |= (PIN_L_EN << 1);
    pin_bitmask |= (PIN_R_EN << 1);
    out_pins_config.pin_bit_mask = pin_bitmask;
    // ESP_LOGI("HalGPIO", "Output pins init bitmask = ");
    out_pins_config.mode = GPIO_MODE_OUTPUT;
    out_pins_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    out_pins_config.pull_up_en = GPIO_PULLUP_DISABLE;
    out_pins_config.intr_type = GPIO_INTR_DISABLE;

    (void) gpio_config(&out_pins_config);

    // Set input GPIO pins

    // Set PWM pins

 }
 
 /******************************************************************************/
 
 int HalGpio_ReadPin (uint16_t gpio_num)
 {
     return gpio_get_level(gpio_num);
 }
 
 /******************************************************************************/
 
 int HalGpio_WritePin (uint16_t gpio_num, uint32_t level)
 {
     return gpio_set_level(gpio_num, level);
 }
 
 /******************************************************************************/
 int HalGpio_TogglePin(uint16_t gpio_num)
 {
     //ToDo: Implement this.
     return 0;
 }
 
 /******************************************************************************/
 /* Local functions                                                            */
 /******************************************************************************/
 
 /**
  * \}
  * End of file.
  */