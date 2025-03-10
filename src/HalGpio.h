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

 /******************************************************************************/
 /* Exported defines and types                                                 */
 /******************************************************************************/
 typedef struct HalGpio_Pin_tag
 {
    uint8_t port;
    uint8_t pin;
 }HalGpio_Pin;

 // GPIO output pins
// GPIO output pin declarations (use extern to prevent multiple definitions)
extern HalGpio_Pin PIN_LED1_DEVKIT;
extern HalGpio_Pin PIN_LED2_DEVKIT;
extern HalGpio_Pin PIN_LED3_DEVKIT;
extern HalGpio_Pin PIN_LED4_DEVKIT;
 #define PIN_L_EN       16
 #define PIN_R_EN       17

 // GPIO input pins
 //#define PIN_BUTTON_DEVKIT GPIO_
 #define PIN_ENC_A      34
 #define PIN_ENC_B      35

 // PWM
 #define PIN_RPWM       18
 #define PIN_LPWM       19
 /******************************************************************************/
 /* Exported data                                                              */
 /******************************************************************************/
 
 /******************************************************************************/
 /* Exported functions                                                         */
 /******************************************************************************/
 void          HalGpio_Init     (void);
 int           HalGpio_ReadPin  (HalGpio_Pin gpio_pin);
 int           HalGpio_WritePin (HalGpio_Pin gpio_pin, uint32_t level);
 int           HalGpio_TogglePin(HalGpio_Pin gpio_pin);
 #endif /* defined __HAL_GPIO_H__ */