#include "HalTim.h"
#include <stdint.h>
#include <nrfx_timer.h>
/* https://docs.nordicsemi.com/bundle/ps_nrf5340/page/timer.html#ariaid-title5 */
#include "nrfx_config.h"
#include <nrf.h>  // Nordic header file for direct register access


static HalTimerCallback user_callback = 0;

void TIMER0_IRQHandler(void) __attribute__((interrupt("IRQ")));
void TIMER0_IRQHandler(void) 
{
    if (NRF_TIMER0->EVENTS_COMPARE[0]) 
    {
        NRF_TIMER0->EVENTS_COMPARE[0] = 0;  // Clear event flag
        // Additional step: Clear the pending interrupt in NVIC
        NVIC_ClearPendingIRQ(TIMER0_IRQn);

        if (user_callback) 
        {
            user_callback();  // Execute user-defined callback
        }
    }
}

void HalTimer_Init(uint32_t frequency_hz) 
{
    // Stop TIMER0 before configuring
    NRF_TIMER0->TASKS_STOP = 1;

    // Set 32-bit mode
    NRF_TIMER0->MODE = TIMER_MODE_MODE_Timer;
    NRF_TIMER0->BITMODE = TIMER_BITMODE_BITMODE_32Bit;

    // Set the timer frequency
    NRF_TIMER0->PRESCALER = 4;  // 1MHz clock (16MHz / 2^4 = 1MHz)
    
    // Set compare register (how often the timer triggers an interrupt)
    uint32_t ticks = (1000000 / frequency_hz);  // 1MHz clock -> 1 tick = 1us
    NRF_TIMER0->CC[0] = ticks;

    // Enable interrupt on compare match
    NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

    int basePri = __get_BASEPRI();

    __set_BASEPRI(2<<(8-__NVIC_PRIO_BITS));

    // Configure NVIC
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    NVIC_SetPriority(TIMER0_IRQn, 2);  // Set a medium priority
    NVIC_EnableIRQ(TIMER0_IRQn);
}

void HalTimer_Start(void) 
{
    NRF_TIMER0->TASKS_CLEAR = 1;  // Reset counter
    NRF_TIMER0->TASKS_START = 1;  // Start the timer
}

void HalTimer_Stop(void) 
{
    NRF_TIMER0->TASKS_STOP = 1;  // Stop the timer
}

uint32_t HalTimer_GetTimeUs(void) 
{
    return NRF_TIMER0->TASKS_CAPTURE[1];  // Read current counter value
}

// Registers a callback function
void HalTimer_RegisterCallback(HalTimerCallback callback) 
{
    user_callback = callback;
}

// Unregisters the callback function (disables the timer ISR execution)
void HalTimer_UnregisterCallback(void) 
{
    user_callback = NULL;
}
