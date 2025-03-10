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

 #include <zephyr/kernel.h>
 #include <zephyr/drivers/gpio.h>

 /******************************************************************************/
 /* Local defines and types                                                    */
 /******************************************************************************/
 typedef enum HAL_GPIO_PIN_MODE_tag
 {
    HAL_GPIO_MODE_OUTPUT = 0,
    HAL_GPIO_MODE_INPUT  = 1
 } halGpioPinMode_enum;

 typedef enum HAL_GPIO_PIN_PULL_tag
 {
    HAL_GPIO_PULL_UP    = 0,
    HAL_GPIO_PULL_DOWN  = 1,
    HAL_GPIO_FLOATING   = 2
 }halGpioPinPullMode_enum;

 /******************************************************************************/
 /* Local function prototypes                                                  */
 /******************************************************************************/
 static void configure_pin(HalGpio_Pin gpio_pin, halGpioPinMode_enum mode, halGpioPinPullMode_enum gpio_pullMode, uint8_t interrupt_en);

 /******************************************************************************/
 /* Local data                                                                 */
 /******************************************************************************/
 // GPIO Device Bindings
 static const struct device *gpio_0_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
 static const struct device *gpio_1_dev = DEVICE_DT_GET(DT_NODELABEL(gpio1));

 HalGpio_Pin PIN_LED1_DEVKIT = {.port=0, .pin=28};
 HalGpio_Pin PIN_LED2_DEVKIT = {.port=0, .pin=29};
 HalGpio_Pin PIN_LED3_DEVKIT = {.port=0, .pin=30};
 HalGpio_Pin PIN_LED4_DEVKIT = {.port=0, .pin=31};

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
    // Set output GPIO pins
    // For this project the following pins are output
    // IO_16 = L_EN
    // IO_17 = R_EN
    
    configure_pin(PIN_LED1_DEVKIT, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DOWN, 0);
    configure_pin(PIN_LED2_DEVKIT, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DOWN, 0);
    configure_pin(PIN_LED3_DEVKIT, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DOWN, 0);

    // https://docs.nordicsemi.com/bundle/ug_nrf5340_dk/page/UG/dk/hw_buttons_leds.html
    // HalGpio_WritePin (PIN_LED2_DEVKIT, 0);
    // HalGpio_WritePin (PIN_LED3_DEVKIT, 0);

    // Set input GPIO pins

    // Set PWM pins

 }
 
 /******************************************************************************/
 
 int HalGpio_ReadPin (HalGpio_Pin gpio_pin)
 {
    if(gpio_pin.port == 0)
    {
       return gpio_pin_get(gpio_0_dev, (gpio_pin_t)gpio_pin.pin);
    }
    else if(gpio_pin.port == 1)
    {
       return gpio_pin_get(gpio_1_dev, (gpio_pin_t)gpio_pin.pin);
    }
    else
    {
       return 0xE;
    }
 }
 
 /******************************************************************************/
 
 int HalGpio_WritePin (HalGpio_Pin gpio_pin, uint32_t level)
 {   
     if(gpio_pin.port == 0)
     {
        return gpio_pin_set(gpio_0_dev, (gpio_pin_t)gpio_pin.pin, (int)level);
     }
     else if(gpio_pin.port == 1)
     {
        return gpio_pin_set(gpio_1_dev, (gpio_pin_t)gpio_pin.pin, (int)level);
     }
     else
     {
        return 0xE;
     }
 }
 
 /******************************************************************************/
 int HalGpio_TogglePin(HalGpio_Pin gpio_pin)
 {
    if(gpio_pin.port == 0)
    {
       return gpio_pin_toggle(gpio_0_dev, (gpio_pin_t)gpio_pin.pin);
    }
    else if(gpio_pin.port == 1)
    {
        return gpio_pin_toggle(gpio_1_dev, (gpio_pin_t)gpio_pin.pin);
    }
    else
    {
       return 0xE;
    }
 }
 
 /******************************************************************************/
 /* Local functions                                                            */
 /******************************************************************************/
/**
 * @brief Configure a GPIO pin with specified settings
 * 
 * @param gpio_pin       GPIO port and pin number
 * @param mode           GPIO mode (0: Input, 1: Output)
 * @param gpio_pullMode  Enable pull-up resistor (0: pullup, 1: pull down, 2: floating)
 * @param interrupt_en   Enable interrupt (1: Enable, 0: Disable)
 */
static void configure_pin(HalGpio_Pin gpio_pin, halGpioPinMode_enum mode, halGpioPinPullMode_enum gpio_pullMode, uint8_t interrupt_en)
{
    if (!device_is_ready(gpio_0_dev)) 
    {
        // LOG_ERR("GPIO device not ready!");
        return;
    }

    int flags = 0;

    // Set mode: Input or Output
    if (mode == HAL_GPIO_MODE_OUTPUT) 
    {
        flags |= GPIO_OUTPUT_INACTIVE | GPIO_ACTIVE_LOW;  // Set output LOW initially
    } 
    else 
    {
        flags |= GPIO_INPUT;
    }

    // Configure Pull-up/Pull-down
    if (gpio_pullMode == HAL_GPIO_PULL_UP) 
    {
        flags |= GPIO_PULL_UP;
    }
    else if (gpio_pullMode == HAL_GPIO_PULL_DOWN) 
    {
        flags |= GPIO_PULL_DOWN;
    }
    else
    {
        // do nothing
    }

    // Configure Interrupts
    if (interrupt_en) 
    {
        flags |= GPIO_INT_ENABLE | GPIO_INT_EDGE_TO_ACTIVE;
    }

    // Apply Configuration
    int ret = 0xE;
    if(gpio_pin.port == 0)
    {
    ret = gpio_pin_configure(gpio_0_dev, (gpio_pin_t)gpio_pin.pin, flags);
    }
    else if(gpio_pin.port == 1)
    {
    ret = gpio_pin_configure(gpio_1_dev, (gpio_pin_t)gpio_pin.pin, flags);
    }

    if (ret < 0) 
    {
        // LOG_ERR("Failed to configure GPIO pin %llu", pin_num);
    } else 
    {
        // LOG_INF("Configured pin %llu as %s", pin_num, mode ? "OUTPUT" : "INPUT");
    }
}
 /**
  * \}
  * End of file.
  */