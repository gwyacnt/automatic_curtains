/******************************************************************************/

/**
 *  \file    HalGpio.c
 *  \author  https://github.com/gwyacnt/
 *  \brief   GPIO component implementation
 *
 *  \remarks 
 */

 // Standard lib includes
 #include <stdint.h>
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>

 // Target specific includes
 #include <zephyr/kernel.h>
 #include <zephyr/drivers/gpio.h>
 #include <zephyr/irq.h>
#include <zephyr/sys/printk.h>
 // GWYACNT includes
 #include "HalGpio.h"
 #include "UtilGen.h"
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
 static void encoder_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
 /******************************************************************************/
 /* Local data                                                                 */
 /******************************************************************************/
 // GPIO Device Bindings
 static const struct device *gpio_0_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
 static const struct device *gpio_1_dev = DEVICE_DT_GET(DT_NODELABEL(gpio1));
 static struct gpio_callback enc_a_cb;
 static struct gpio_callback enc_b_cb;
 static volatile int32_t motor_position = 0;  // Motor position counter
 static HalGpioCallback user_callback = NULL;

/******************************************************************************/
/* Exported data                                                              */
/******************************************************************************/
// Pin assignments
 HalGpio_Pin PIN_LED1_DEVKIT = {.port=0, .pin=28};
 HalGpio_Pin PIN_LED2_DEVKIT = {.port=0, .pin=29};
 HalGpio_Pin PIN_LED3_DEVKIT = {.port=0, .pin=30};
 HalGpio_Pin PIN_LED4_DEVKIT = {.port=0, .pin=31};
 HalGpio_Pin PIN_MOTOR_R_EN  = {.port=0, .pin=16};
 HalGpio_Pin PIN_MOTOR_L_EN  = {.port=0, .pin=17};
 HalGpio_Pin PIN_MOTOR_R_PWM = {.port=0, .pin=18};
 HalGpio_Pin PIN_MOTOR_L_PWM = {.port=0, .pin=19};
 HalGpio_Pin PIN_ENC_A       = {.port=0, .pin=23};
 HalGpio_Pin PIN_ENC_B       = {.port=0, .pin=24};

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
    // Set input GPIO pins
    configure_pin(PIN_ENC_A, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_UP, 1);
    configure_pin(PIN_ENC_B, HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_UP, 1);
    // Set up interrupt on ENC_A
    gpio_pin_interrupt_configure(gpio_0_dev, PIN_ENC_A.pin, GPIO_INT_EDGE_BOTH);
    gpio_init_callback(&enc_a_cb, encoder_handler, (1UL << (PIN_ENC_A.pin)));
    gpio_add_callback(gpio_0_dev, &enc_a_cb);

    // Set PWM pins
    
    
    // Debugging LEDs
    // https://docs.nordicsemi.com/bundle/ug_nrf5340_dk/page/UG/dk/hw_buttons_leds.html
    configure_pin(PIN_LED1_DEVKIT, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DOWN, 0);
    configure_pin(PIN_LED2_DEVKIT, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DOWN, 0);
    configure_pin(PIN_LED3_DEVKIT, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DOWN, 0);
    configure_pin(PIN_LED4_DEVKIT, HAL_GPIO_MODE_OUTPUT, HAL_GPIO_PULL_DOWN, 0);
    
    // HalGpio_WritePin (PIN_LED2_DEVKIT, 0);
    // HalGpio_WritePin (PIN_LED3_DEVKIT, 0);
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
       return UTIL_GEN_ERROR_GENERIC;
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
        return UTIL_GEN_ERROR_GENERIC;
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
        return UTIL_GEN_ERROR_GENERIC;
    }
}
/******************************************************************************/

void  HalGpio_RegisterCallback   (HalGpioCallback callback)
{
    user_callback = callback;
}
/******************************************************************************/

void  HalGpio_UnregisterCallback (void)
{
    user_callback = NULL;
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
        flags |= GPIO_INPUT ;
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
        flags |= GPIO_INT_ENABLE | GPIO_INT_EDGE ;
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
/******************************************************************************/

static void encoder_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    if (user_callback) 
    {
        user_callback();  // Execute user-defined callback
    }
}

/**
 * \}
 * End of file.
 */