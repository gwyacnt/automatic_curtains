/******************************************************************************/

/**
 *  \file    HalTim.h
 *  \author  https://github.com/gwyacnt/
 *  \brief   Timer component definitions
 *
 *  \remarks 
 */

#ifndef __HAL_TIMER_H
#define __HAL_TIMER_H

#include <stdint.h>
#include <nrfx_timer.h>

/******************************************************************************/
/* Exported defines and types                                                 */
/******************************************************************************/
typedef void (*HalTimerCallback)(void);  // Callback function type

/******************************************************************************/
/* Exported data                                                              */
/******************************************************************************/

/******************************************************************************/
/* Exported functions                                                         */
/******************************************************************************/
void        HalTimer_Init               (void);
void        HalTimer_Start              (uint32_t timeout_us);
void        HalTimer_Stop               (void);
uint32_t    HalTimer_GetTimeUs          (void);
void        HalTimer_RegisterCallback   (HalTimerCallback callback);
void        HalTimer_UnregisterCallback (void);

#endif
