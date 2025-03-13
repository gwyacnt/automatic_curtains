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
extern HalGpio_Pin PIN_LED1_DEVKIT; // P0.28
extern HalGpio_Pin PIN_LED2_DEVKIT; // P0.29
extern HalGpio_Pin PIN_LED3_DEVKIT; // P0.30
extern HalGpio_Pin PIN_LED4_DEVKIT; // P0.31

//R_EN and L_EN must be set HIGH to enable movement.
extern HalGpio_Pin PIN_MOTOR_R_EN; // P0.16
extern HalGpio_Pin PIN_MOTOR_L_EN; // P0.17

//RPWM and LPWM are controlled using PWM signals to adjust speed and direction.
extern HalGpio_Pin PIN_MOTOR_R_PWM; // P0.18
extern HalGpio_Pin PIN_MOTOR_L_PWM; // P0.19

// OUT C1/A and OUT C2/B provide quadrature signals to track motor position & speed.
// These pins are read using interrupts or timers in nRF5340.
extern HalGpio_Pin PIN_ENC_A;       // P0.14
extern HalGpio_Pin PIN_ENC_B;       // P0.15
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