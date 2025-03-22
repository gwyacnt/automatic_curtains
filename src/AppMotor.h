/******************************************************************************/

/**
 *  \file    AppMotor.h
 *  \author  https://github.com/gwyacnt/
 *  \brief   App Motor component definitions
 *
 *  \remarks 
 */

#ifndef __APP_MOTOR_H__
#define __APP_MOTOR_H__

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/* Exported defines and types                                                 */
/******************************************************************************/
typedef enum MotorDirection_tag
{
    MOTOR_DIRECTION_CW,
    MOTOR_DIRECTION_CCW,
    MOTOR_DIRECTION_STOPPED
} MotorDirection_enum_t;

/******************************************************************************/
/* Exported data                                                              */
/******************************************************************************/

/******************************************************************************/
/* Exported functions                                                         */
/******************************************************************************/
void          AppMotor_SetTarget         (int64_t setpoint);
int64_t       AppMotor_GetTarget         (void);
void          AppMotor_Halt              (void);
void          AppMotor_CalculatePID      (double* controlSignal, double* errorValue);
void          AppMotor_SetPidCoeffs      (double P, double I, double D);
void          AppMotor_DriveMotorTask    (void);
void          AppMotor_SetSpeed          (MotorDirection_enum_t direction, int speed_percent);
#endif /* defined __APP_MOTOR_H__ */
