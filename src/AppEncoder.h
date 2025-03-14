/******************************************************************************/

/**
 *  \file    AppEncoder.h
 *  \author  https://github.com/gwyacnt/
 *  \brief   App Encoder component definitions
 *
 *  \remarks 
 */

#ifndef __APP_ENCODER_H__
#define __APP_ENCODER_H__

#include <stdint.h>

void       AppEncoder_Init         (void);
int64_t    AppEncoder_GetPosition  (void);
void       AppEncoder_SetPosition  (int64_t pos);
void       AppEncoder_InterruptHandler  (void);

#endif