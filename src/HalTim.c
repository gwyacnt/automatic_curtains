/******************************************************************************/

/**
 *  \file    HalTim.c
 *  \author  https://github.com/gwyacnt/
 *  \brief   Timer component implementation
 *
 *  \remarks  https://docs.nordicsemi.com/bundle/ps_nrf5340/page/timer.html#ariaid-title5
 */

// Standard lib includes
#include <stdint.h>
#include <stdio.h>

// Target specific includes
#include <nrfx_timer.h>
#include <nrf5340_application_bitfields.h>

// GWYACNT includes
#include "HalTim.h"
/******************************************************************************/
/* Local defines and types                                                    */
/******************************************************************************/
/** @brief Symbol specifying time in milliseconds to wait for handler execution. */
#define TIME_TO_WAIT_MS 100UL
/******************************************************************************/
/* Local data                                                                 */
/******************************************************************************/
// Get a reference to the TIMER0 instance
static const nrfx_timer_t timer_0_inst = NRFX_TIMER_INSTANCE(0);
static HalTimerCallback user_callback = 0;

/******************************************************************************/
/* Local function prototypes                                                  */
/******************************************************************************/
/**
 * @brief Function for handling TIMER driver events.
 *
 * @param[in] event_type Timer event.
 * @param[in] p_context  General purpose parameter set during initialization of
 *                       the timer. This parameter can be used to pass
 *                       additional information to the handler function, for
 *                       example, the timer ID.
 */
static void timer_handler(nrf_timer_event_t event_type, void * p_context);

/******************************************************************************/
/* Exported data                                                              */
/******************************************************************************/

/******************************************************************************/
/* Exported functions                                                         */
/******************************************************************************/
void HalTimer_Init(void) 
{
    nrfx_err_t status;
    (void)status;

    uint32_t base_frequency = NRF_TIMER_BASE_FREQUENCY_GET(timer_inst.p_reg);
    printf("\ntimer base frequency: %u\n", base_frequency);
    nrfx_timer_config_t config = NRFX_TIMER_DEFAULT_CONFIG(base_frequency);
    config.bit_width = NRF_TIMER_BIT_WIDTH_32;
    // config.p_context = "Some context";

    status = nrfx_timer_init(&timer_0_inst, &config, timer_handler);
    if (status != NRFX_SUCCESS) 
    {
		printf("\nError initializing timer: %x\n", status);
	}

    nrfx_timer_clear(&timer_0_inst);

    IRQ_DIRECT_CONNECT(TIMER0_IRQn, 6, nrfx_timer_0_irq_handler, 0);
	irq_enable(TIMER0_IRQn);
}
/******************************************************************************/

void HalTimer_Start(uint32_t timeout_us) 
{
    printf("\nTime to wait: %u ms", timeout_us);

    nrfx_timer_enable(&timer_0_inst);
    printf("\nTimer status: %s", nrfx_timer_is_enabled(&timer_0_inst) ? "enabled" : "disabled");

    /* Creating variable desired_ticks to store the output of nrfx_timer_ms_to_ticks function */
    uint32_t desired_ticks = nrfx_timer_ms_to_ticks(&timer_0_inst, timeout_us);
    // printf("\ndesired_ticks: %u tick", desired_ticks);

    nrfx_timer_extended_compare(&timer_0_inst, NRF_TIMER_CC_CHANNEL0, desired_ticks, 
        NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
}
/******************************************************************************/

void HalTimer_Stop(void) 
{
    NRF_TIMER0->TASKS_STOP = 1;  // Stop the timer
}
/******************************************************************************/

uint32_t HalTimer_GetTimeUs(void) 
{
    return NRF_TIMER0->TASKS_CAPTURE[1];  // Read current counter value
}
/******************************************************************************/

// Registers a callback function
void HalTimer_RegisterCallback(HalTimerCallback callback) 
{
    user_callback = callback;
}
/******************************************************************************/

// Unregisters the callback function (disables the timer ISR execution)
void HalTimer_UnregisterCallback(void) 
{
    user_callback = NULL;
}

/******************************************************************************/
/* Local functions                                                            */
/******************************************************************************/
static void timer_handler(nrf_timer_event_t event_type, void * p_context)
{
    if(event_type == NRF_TIMER_EVENT_COMPARE0)
    {
        if (user_callback) 
        {
            // char * p_msg = p_context;
            // printf("\nTimer finished. Context passed to the handler: >%s<", p_msg);
            user_callback();  // Execute user-defined callback
        }
    }
}


/**
 * \}
 * End of file.
 */