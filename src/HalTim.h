#ifndef __HAL_TIMER_H
#define __HAL_TIMER_H

#include <stdint.h>

typedef void (*HalTimerCallback)(void);  // Callback function type

void        HalTimer_Init               (uint32_t frequency_hz);
void        HalTimer_Start              (void);
void        HalTimer_Stop               (void);
uint32_t    HalTimer_GetTimeUs          (void);
void        HalTimer_RegisterCallback   (HalTimerCallback callback);
void        HalTimer_UnregisterCallback (void);

#endif // __HAL_TIMER_H#ifndef TIMER_H
