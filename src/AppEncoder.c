#include <stdint.h>
#include <stdio.h>
#include "AppEncoder.h"
#include "HalGpio.h"

static int64_t motor_position = 0;

void AppEncoder_Init (void)
{
    // Register GPIO encoder pin callback function
    HalGpio_RegisterCallback(AppEncoder_InterruptHandler);
}

int64_t AppEncoder_GetPosition  (void)
{
    return motor_position;
}

void AppEncoder_SetPosition  (int64_t pos)
{
    motor_position = pos;
}

void AppEncoder_InterruptHandler (void)
{
    int enc_a = HalGpio_ReadPin(PIN_ENC_A);
    int enc_b = HalGpio_ReadPin(PIN_ENC_B);

    // Determine direction
    if (enc_a == enc_b) 
    {
        motor_position++;  // Clockwise
    } else 
    {
        motor_position--;  // Counterclockwise
    }

    printf("\n******* pos: %lld \n", motor_position);
}