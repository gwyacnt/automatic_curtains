/******************************************************************************/

/**
 *  \file    HalGpio.h
 *  \author  https://github.com/gwyacnt/
 *  \brief   GPIO component definitions
 *
 *  \remarks 
 */

 #ifndef __HAL_GPIO_H__
 #define __HAL_GPIO_H__
 
 #include <stdint.h>
 #include <stdbool.h>
 #include "driver/gpio.h"

 /******************************************************************************/
 /* Exported defines and types                                                 */
 /******************************************************************************/
 // Most ESP32 dev kits have an LED on GPIO 2
 #define PIN_LED_DEVKIT GPIO_NUM_2
 #define PIN_L_EN       GPIO_NUM_16
 #define PIN_R_EN       GPIO_NUM_17

 /******************************************************************************/
 /* Exported data                                                              */
 /******************************************************************************/
 
 /******************************************************************************/
 /* Exported functions                                                         */
 /******************************************************************************/
 void          HalGpio_Init     (void);
 int           HalGpio_ReadPin  (uint16_t gpio_num);
 int           HalGpio_WritePin (uint16_t gpio_num, uint32_t level);
 int           HalGpio_TogglePin(uint16_t gpio_num);
 #endif /* defined __HAL_GPIO_H__ */