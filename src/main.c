

// #include "App.h"

//  int main(void)
//  {
//     AppProcess();
//     return 0;
//  }

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

void print_menu(void) {
    printk("\n===== Interactive Menu =====\n");
    printk("Type a command and press Enter:\n");
    printk("> led on  - Turn LED ON\n");
    printk("> led off - Turn LED OFF\n");
    printk("> status  - Get status\n");
    printk("> exit    - Quit\n");
    printk("Your input: ");
}

// Process full command from buffer
void process_input(struct k_work *work) {
    rx_buffer[buffer_pos] = '\0';  // Null-terminate the string
    printk("\nReceived: %s\n", rx_buffer);

    if (strcmp(rx_buffer, "led on") == 0) {
        printk("Turning LED ON...\n");
        // Add LED ON logic here
    } else if (strcmp(rx_buffer, "led off") == 0) {
        printk("Turning LED OFF...\n");
        // Add LED OFF logic here
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

void main(void) {
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

    printk("\nSerial Interactive Menu Started!\n");
    print_menu();

    while (1) {
        k_sleep(K_SECONDS(1));  // Keep main loop running
        printk(".");  // Simulate background process
    }
}



/*
### **🚀 Sending a Full Sentence Over Serial (UART) in nRF5340**
Yes! Instead of **reading a single character**, you can **buffer a full sentence** (up to `N` characters) and process it once the user **presses Enter (`\n`)**.

---

## **✅ 1️⃣ Key Changes for Sentence Input**
| **Feature** | **How It Works?** |
|------------|------------------|
| **Buffer Input Instead of Single Char** | Stores received characters in an array (`rx_buffer[]`). |
| **Detect `\n` (Enter Key)** | Processes input when user presses **Enter (`\n`)**. |
| **Ignore Backspace (`\b`)** | Allows correction during typing. |
| **Limit Input Length** | Avoids buffer overflow. |

---

## **✅ 2️⃣ Updated Code to Read a Sentence**
### **📌 `main.c` (Non-Blocking UART with Sentence Input)**
```c
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

void print_menu(void) {
    printk("\n===== Interactive Menu =====\n");
    printk("Type a command and press Enter:\n");
    printk("> led on  - Turn LED ON\n");
    printk("> led off - Turn LED OFF\n");
    printk("> status  - Get status\n");
    printk("> exit    - Quit\n");
    printk("Your input: ");
}

// Process full command from buffer
void process_input(struct k_work *work) {
    rx_buffer[buffer_pos] = '\0';  // Null-terminate the string
    printk("\nReceived: %s\n", rx_buffer);

    if (strcmp(rx_buffer, "led on") == 0) {
        printk("Turning LED ON...\n");
        // Add LED ON logic here
    } else if (strcmp(rx_buffer, "led off") == 0) {
        printk("Turning LED OFF...\n");
        // Add LED OFF logic here
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

void main(void) {
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

    printk("\nSerial Interactive Menu Started!\n");
    print_menu();

    while (1) {
        k_sleep(K_SECONDS(1));  // Keep main loop running
        printk(".");  // Simulate background process
    }
}
```

---

## **✅ 3️⃣ Key Improvements**
| **Feature** | **How It Works?** |
|------------|------------------|
| **Full sentence input (up to 32 chars)** | User types a full command before processing. |
| **Press `Enter` to send the command** | `\n` or `\r` triggers command execution. |
| **Handles Backspace (`\b`)** | Allows user to correct input. |
| **Processes commands asynchronously** | Uses `k_work_submit()` to handle input in background. |

---

## **✅ 4️⃣ Example Output**
```
===== Interactive Menu =====
Type a command and press Enter:
> led on  - Turn LED ON
> led off - Turn LED OFF
> status  - Get status
> exit    - Quit
Your input: . . . 

led on
Received: led on
Turning LED ON...

===== Interactive Menu =====
Your input: . . . 

status
Received: status
System is running normally.

===== Interactive Menu =====
Your input: 
```
✅ **Fully interactive, non-blocking menu!**  
✅ **Handles full sentences instead of characters!**  

---

## **📌 Summary**
| **Feature** | **Benefit** |
|------------|------------|
| **Non-blocking UART (`uart_irq_rx_enable()`)** | Allows serial input **without blocking the main loop** |
| **Full Sentence Input Buffer (`rx_buffer[]`)** | User **types full command and presses Enter** |
| **Work Queue (`k_work_submit()`)** | Processes input asynchronously |
| **Handles Backspace (`\b`)** | Allows user to **edit before sending** |

---

## **🚀 Next Steps**
🔹 **Want to add real LED control logic?**  
🔹 **Need a menu for controlling motor speed?**  
🔹 **Want to store settings in Flash memory?**  

Let me know how I can assist! 🚀🔥

*/