#ifndef __HAL_TIMER_H
#define __HAL_TIMER_H

#include <stdint.h>
#include <nrfx_timer.h>
typedef void (*HalTimerCallback)(void);  // Callback function type

void        HalTimer_Init               (void);
void        HalTimer_Start              (uint32_t timeout_us);
void        HalTimer_Stop               (void);
uint32_t    HalTimer_GetTimeUs          (void);
void        HalTimer_RegisterCallback   (HalTimerCallback callback);
void        HalTimer_UnregisterCallback (void);
void timer_handler(nrf_timer_event_t event_type, void * p_context);
#endif // __HAL_TIMER_H#ifndef TIMER_H
