/******************************************************************************/

/**
 *  \file    AppMotor.c
 *  \author  https://github.com/gwyacnt/
 *  \brief   AppMotor component implementation
 *
 *  \remarks https://curiousscientist.tech/blog/dc-motor-position-control-using-pid
*/

// Standard lib includes
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Target specific includes
#include <zephyr/kernel.h>

// GWYACNT includes
#include "AppMotor.h"
#include "AppEncoder.h"
#include "HalGpio.h"
#include "HalPwm.h"

/******************************************************************************/
/* Local defines and types                                                    */
/******************************************************************************/


/******************************************************************************/
/* Local data                                                                 */
/******************************************************************************/
/* PID parameters */
static double PID_PROPORTIONAL_COEFF = 1.35; //k_p = 0.5
static double PID_INTEGRAL_COEFF = 0.01; //k_d = 1
static double PID_DERIVATIVE_COEFF = 0.00005; //k_i = 3
static double  errorIntegral;    // integral term - Newton-Leibniz, notice, this is a running sum!
static double  previousError;    // save the error for the next iteration to get the difference (for edot)
static int64_t previousTime;

/* Motor Driving parameters */
static int64_t setpoint;
static uint8_t maxSpeed_percent;
static uint8_t minSpeed_percent;

/******************************************************************************/
/* Local function prototypes                                                  */
/******************************************************************************/

/******************************************************************************/
/* Exported functions                                                         */
/******************************************************************************/
void AppMotor_SetPidCoeffs (double P, double I, double D)
{
    PID_PROPORTIONAL_COEFF = P;
    PID_INTEGRAL_COEFF     = I;
    PID_DERIVATIVE_COEFF   = D;
}
/******************************************************************************/

void AppMotor_SetTarget (int64_t target_setpoint)
{
    setpoint = target_setpoint;
}
/******************************************************************************/

int64_t AppMotor_GetTarget (void)
{
    return setpoint;
}
/******************************************************************************/

void AppMotor_DriveMotorTask (void)
{
    double control_pwm, errorValue;
    MotorDirection_enum_t motorDirection;

    AppMotor_CalculatePID(&control_pwm, &errorValue);
    //Determine speed and direction based on the value of the control signal
    //direction
    if (control_pwm < 0) //negative value: CCW
    {
        motorDirection = MOTOR_DIRECTION_CCW;
    }
    else if (control_pwm > 0) //positive: CW
    {
        motorDirection = MOTOR_DIRECTION_CW;
    }
    else //0: STOP - this might be a bad practice when you overshoot the setpoint
    {
        motorDirection = MOTOR_DIRECTION_STOPPED;
    }

    //---------------------------------------------------------------------------
    //Speed
    //PWM values cannot be negative and have to be integers
    uint64_t PWMValue = (uint64_t)fabs(control_pwm);

    // 0-100
    PWMValue = PWMValue/UINT64_MAX *100;

    // checking speed limits
    if (PWMValue > maxSpeed_percent)
    {
        PWMValue = maxSpeed_percent;
    }

    if (PWMValue < minSpeed_percent && errorValue != 0)
    {
        PWMValue = minSpeed_percent;
    }
    //A little explanation for the "bottom capping":
    //Under a certain PWM value, there won't be enough current flowing through the coils of the motor
    //Therefore, despite the fact that the PWM value is set to the "correct" value, the motor will not move
    //The above value is an empirical value, it depends on the motors perhaps, but 30 seems to work well in my case

    /*  Set Direction and motor speed  */
    if (motorDirection == MOTOR_DIRECTION_CCW) //-1 == CCW
    {
        HalGpio_WritePin(PIN_MOTOR_R_EN, 0);
        HalGpio_WritePin(PIN_MOTOR_L_EN, 1);
        PWM_SetDutyCycle(PIN_MOTOR_L_PWM.pin, PWMValue);
    }
    else if (motorDirection == MOTOR_DIRECTION_CW) // == 1, CW
    {
        HalGpio_WritePin(PIN_MOTOR_R_EN, 1);
        HalGpio_WritePin(PIN_MOTOR_L_EN, 0);
        PWM_SetDutyCycle(PIN_MOTOR_R_PWM.pin, PWMValue);
    }
    else // == MOTOR_DIRECTION_STOPPED, stop/break
    {
        HalGpio_WritePin(PIN_MOTOR_R_EN, 0);
        HalGpio_WritePin(PIN_MOTOR_L_EN, 0);
        PWMValue = 0;
        PWM_SetDutyCycle(PIN_MOTOR_R_PWM.pin, PWMValue);
        PWM_SetDutyCycle(PIN_MOTOR_L_PWM.pin, PWMValue);
        //In this block we also shut down the motor and set the PWM to zero
    }

    //----------------------------------------------------
    //Optional printing on the terminal to check what's up
    printf("\n errorValue: %f",errorValue);
    printf("\n PWMValue: %lld", PWMValue);
    printf("\n targetPosition: %lld", setpoint);
}
/******************************************************************************/

/*
returns:
    double controlSignal;     // u - Also called as process variable (PV)
    double errorValue;        // Current position - target position (or setpoint)
*/
void AppMotor_CalculatePID (double* controlSignal, double* errorValue)
{
    
    double edot;             // edot = de/dt - derivative term
    int64_t motor_position; 

    /*  Determining the elapsed time   */
    // current time
    int64_t currentTime = k_uptime_get();
    // time difference in seconds
    int64_t deltaTime_s = (currentTime - previousTime) / 1000.0;
    // save the current time for the next iteration to get the time difference
    previousTime = currentTime; 

    /*  Calculate PID parameters   */
    //Current position - target position (or setpoint)
    motor_position = AppEncoder_GetPosition();
    *errorValue = motor_position - setpoint; 
    //edot = de/dt - derivative term
    edot = (*errorValue - previousError) / deltaTime_s;
    // integral term - Newton-Leibniz, notice, this is a running sum!
    errorIntegral = errorIntegral + (*errorValue * deltaTime_s); 
    // final sum, proportional term also calculated here
    *controlSignal = (PID_PROPORTIONAL_COEFF * *errorValue) + (PID_DERIVATIVE_COEFF * edot) + (PID_INTEGRAL_COEFF * errorIntegral); 
    //save the error for the next iteration to get the difference (for edot)
    previousError = *errorValue; 

    //----------------------------------------------------
    //Optional printing on the terminal to check what's up
    printf("\n motorPosition: %lld", motor_position);
}



void  AppMotor_SetSpeed(int speed_percent)
{
    
    if (speed_percent == 0) // Stop
    {
        HalGpio_WritePin(PIN_MOTOR_R_EN, 0);
        HalGpio_WritePin(PIN_MOTOR_L_EN, 0);
        PWM_SetDutyCycle(1, 0);
        PWM_SetDutyCycle(2, 0);
        printf("\nStopping");
    }
    else if( speed_percent > 0 ) // Clockwise
    {
        // Cap speed
        if (speed_percent > 100) speed_percent = 100;
        // Disable CCW
        HalGpio_WritePin(PIN_MOTOR_L_EN, 0);
        PWM_SetDutyCycle(2, 0);
        // Set CW
        HalGpio_WritePin(PIN_MOTOR_R_EN, 1);
        PWM_SetDutyCycle(1, speed_percent);
        printf("\nClockwise with %d speed", speed_percent);
    }
    else if( speed_percent < 0)
    {
        // Cap speed
        if (speed_percent < -100) speed_percent = -100;
        // Disable CW
        HalGpio_WritePin(PIN_MOTOR_R_EN, 0);
        PWM_SetDutyCycle(1, 0);
        // Set CCW
        speed_percent = speed_percent*-1;
        HalGpio_WritePin(PIN_MOTOR_L_EN, 1);
        PWM_SetDutyCycle(2, speed_percent);
        printf("\nCounter-clockwise with %d speed", speed_percent);
    }
}
/******************************************************************************/
/******************************************************************************/
/* Local functions                                                            */
/******************************************************************************/

/**
 * \}
 * End of file.
 */