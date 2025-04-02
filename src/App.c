

#include <stdio.h>
#include "HalGpio.h"
#include "UtilGen.h"
#include "SrvScheduler.h"
#include "HalPwm.h"
#include "HalTim.h"
#include "AppEncoder.h"
#include "AppMotor.h"
#include <zephyr/sys/printk.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <string.h>

#define UART_DEVICE_NODE DT_CHOSEN(zephyr_console)
#define MAX_INPUT_LEN 32  // Max sentence length

static const struct device *uart_dev;
static struct k_work input_work;

static char rx_buffer[MAX_INPUT_LEN];  // Buffer for full sentence
static uint8_t buffer_pos = 0;         // Position in buffer
static int8_t motor_pwm = 0;

void print_menu(void) {
    printk("\n===== Interactive Menu =====\n");
    printk("Type a command and press Enter:\n");
    printk("Your input: ");
}

// Process full command from buffer
void process_input(struct k_work *work) 
{
    rx_buffer[buffer_pos] = '\0';  // Null-terminate the string
    printk("\nReceived: %s\n", rx_buffer);

    if (strcmp(rx_buffer, "led 50") == 0) 
    {
        printk("Turning LEDs 0 and 3 with 50 percent...\n");
        PWM_SetDutyCycle(HAL_PWM_CHANNEL_LED3_DEVKIT, 50);
        PWM_SetDutyCycle(HAL_PWM_CHANNEL_LED4_DEVKIT, 50);

    // } else if (strcmp(rx_buffer, "led 0") == 0) 
    // {
    //     printk("Turning LEDs 0 and 3 with 0 percent...\n");
    //     PWM_SetDutyCycle(0, 0);
    //     PWM_SetDutyCycle(3, 0);
    
    // } else if (strcmp(rx_buffer, "led 100") == 0) 
    // {
    //     printk("Turning LEDs 0 and 3 with 100 percent...\n");
    //     PWM_SetDutyCycle(0, 100);
    //     PWM_SetDutyCycle(3, 100);

    // } else if (strcmp(rx_buffer, "R100") == 0) 
    // {
    //     printk("CW 100 ...\n");
    //     AppMotor_SetSpeed(MOTOR_DIRECTION_CW, 100);
    // } else if (strcmp(rx_buffer, "R50") == 0) 
    // {
    //     printk("CW 50 ...\n");
    //     AppMotor_SetSpeed(MOTOR_DIRECTION_CW, 50);

    // } else if (strcmp(rx_buffer, "S") == 0) 
    // {
    //     printk("Stopping ...\n");
    //     AppMotor_SetSpeed(MOTOR_DIRECTION_STOPPED, 0);
    // } else if (strcmp(rx_buffer, "L50") == 0) 
    // {
    //     printk("CCW 50 ...\n");
    //     AppMotor_SetSpeed(MOTOR_DIRECTION_CCW, 50);
    // } else if (strcmp(rx_buffer, "L100") == 0) 
    // {
    //     printk("CCW 100 ...\n");
    //     AppMotor_SetSpeed(MOTOR_DIRECTION_CCW, 100);

    // } else if (strcmp(rx_buffer, "R 0") == 0) 
    // {
    //     printk("R_EN 0...\n");
    //     HalGpio_WritePin(PIN_MOTOR_R_EN, 0);
    // } else if (strcmp(rx_buffer, "R 1") == 0) 
    // {
    //     printk("R_EN 1...\n");
    //     HalGpio_WritePin(PIN_MOTOR_R_EN, 1);
    } else if (strcmp(rx_buffer, "u") == 0) 
    {
        motor_pwm = motor_pwm + 20;
        printk("motor pwm = %d%%\n", motor_pwm);
        AppMotor_SetSpeed(motor_pwm);
    } else if (strcmp(rx_buffer, "d") == 0) 
    {
        motor_pwm = motor_pwm - 20;
        printk("motor pwm = %d%%\n", motor_pwm);
        AppMotor_SetSpeed(motor_pwm);

    } else if (strcmp(rx_buffer, "status") == 0) {
        printk("System is running normally.\n");
    } else if (strcmp(rx_buffer, "exit") == 0) {
        printk("Exiting...\n");
        return;
    } else {
        printk("Unknown command: %s\n", rx_buffer);
    }

    // Reset buffer for new input
    buffer_pos = 0;
    memset(rx_buffer, 0, MAX_INPUT_LEN);
    print_menu();
}

// UART interrupt handler (non-blocking input)
void uart_interrupt_handler(const struct device *dev, void *user_data) {
    char received_char;
    if (!uart_irq_update(dev)) {
        return;
    }

    if (uart_irq_rx_ready(dev)) {
        uart_poll_in(dev, &received_char);

        if (received_char == '\n' || received_char == '\r') {  // Enter key pressed
            k_work_submit(&input_work);  // Process input
        } else if (received_char == '\b' && buffer_pos > 0) {  // Handle backspace
            buffer_pos--;
            rx_buffer[buffer_pos] = '\0';
        } else if (buffer_pos < MAX_INPUT_LEN - 1) {  // Add character to buffer
            rx_buffer[buffer_pos++] = received_char;
        }
    }
}

void tastBlinkLED(void)
{
    // static uint64_t last_time_ms = 0;  // Store last execution time
    
    // uint64_t current_time_ms = k_uptime_get();  // Get current system time
    // if (last_time_ms != 0) 
    // {
    //     uint64_t elapsed = current_time_ms - last_time_ms;
    //     printk(" - Elapsed time since last execution: %llu ms\n", elapsed);
    // }
    // last_time_ms = current_time_ms;  // Update last execution time

    HalGpio_TogglePin (PIN_LED1_DEVKIT);
}


void AppProcess(void)
{
     HalGpio_Init();
     PWM_Init();
     SrvScheduler_Init();
     SrvScheduler_AddTask(tastBlinkLED, 1000);
     SrvScheduler_Start();
     AppEncoder_Init();

     uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
     if (!device_is_ready(uart_dev)) {
         printk("UART device not ready!\n");
         return;
     }
 
     // Initialize work queue for processing input
     k_work_init(&input_work, process_input);
 
     // Configure UART for interrupt-driven operation
     uart_irq_callback_user_data_set(uart_dev, uart_interrupt_handler, NULL);
     uart_irq_rx_enable(uart_dev);

     printk("\n\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n");
     printk("\nSerial Interactive Menu Started!\n");
     print_menu();

     AppMotor_SetTarget (AppEncoder_ConvertNumOfRotationsToPos(5));
 
     while (1) {
        printk("\n");
        AppMotor_DriveMotorTask();
        k_sleep(K_SECONDS(1));  // Keep main loop running
        // int64_t pos = AppEncoder_GetPosition();
        // if (pos!= 0 && pos % (11*90*2) == 0)
        // {
        //     AppMotor_SetSpeed(0);
        //     motor_pwm = 0;
        //     test_to_reach_full_rotation+=1;
        //     printf("\n pos%%11 reached %lld - test_to_full_rot = %lld", pos, test_to_reach_full_rotation);
        // }
        //  k_sleep(K_SECONDS(1));  // Keep main loop running
        //  printk(".");  // Simulate background process
     }
 
    //  while (1) 
    //  {
    //     // if(HalGpio_ReadPin(PIN_ENC_A) == 1)
    //     // {
    //     //     printk("H\n");
    //     // }
    //     // else if(HalGpio_ReadPin(PIN_ENC_A) == 0)
    //     // {
    //     //     printk("L\n");
    //     // }
    //     //  ret = HalGpio_TogglePin(PIN_LED1_DEVKIT);
    //     // HalGpio_WritePin (PIN_LED1_DEVKIT, 0);
    //     // UtilGen_delay_ms(SLEEP_TIME_MS);
    //     // HalGpio_WritePin (PIN_LED1_DEVKIT, 1);
    //     // UtilGen_delay_ms(SLEEP_TIME_MS);
    //  }
}