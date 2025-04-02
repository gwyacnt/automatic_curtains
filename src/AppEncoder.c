#include <stdint.h>
#include <stdio.h>
#include "AppEncoder.h"
#include "HalGpio.h"

#define ENCODER_PULSES_PER_UNGEARED_REVOLUTIONS 11
#define ENCODER_GEAR_RATIO                      90
#define EDGE_DETECTION_MULTIPLIER               2
#define INTERRUPTS_PER_FULL_REVOLUTION          (ENCODER_PULSES_PER_UNGEARED_REVOLUTIONS*ENCODER_GEAR_RATIO*EDGE_DETECTION_MULTIPLIER)


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

int64_t AppEncoder_ConvertNumOfRotationsToPos (int64_t pos_in_number_of_rotations)
{
    return pos_in_number_of_rotations*INTERRUPTS_PER_FULL_REVOLUTION;
}


void AppEncoder_InterruptHandler (void)
{
    int enc_a = HalGpio_ReadPin(PIN_ENC_A);
    int enc_b = HalGpio_ReadPin(PIN_ENC_B);

    // printf("\nEncoder Interrupt enc_a pin: %d, enc_b pin: %d \n", enc_a, enc_b);

    // Determine direction
    if (enc_a == enc_b) 
    {
        motor_position--;  // Counterclockwise
        // if (motor_position!= 0 && motor_position % INTERRUPTS_PER_FULL_REVOLUTION == 0)
        // {
        //     motor_position_full_revs-=1;
        // }
    } else 
    {
        motor_position++;  // Clockwise
        // if (motor_position!= 0 && motor_position % INTERRUPTS_PER_FULL_REVOLUTION == 0)
        // {
        //     motor_position_full_revs+=1;
        // }
    }
    // printf("\n%lld \n", motor_position);
}



/*
1. Encoder Pulses Per Motor Shaft Rotation (PPR)
From your motor's spec sheet (in the image you shared):

Encoder resolution: 11 PPR (Pulses Per Revolution of the motor shaft)

It's a quadrature encoder (AB phase) → each edge gives an interrupt

✅ 2. Quadrature Mode: How Many Counts per Pulse?
If you're using:

Mode	Counts per PPR
Only rising edge of A	11 (1x)
Both edges of A	22 (2x)
All edges of A and B	44 (4x) ← most accurate
👉 If your code triggers on both edges of A and reads B, you’re likely running in 2x mode.

✅ 3. Gear Reduction
From your image:

Gearbox is applied after encoder

If gear ratio is e.g. 1:30, then:

Total Interrupts per Shaft Rotation
=
PPR
×
Gear Ratio
×
Quadrature Multiplier
Total Interrupts per Shaft Rotation=PPR×Gear Ratio×Quadrature Multiplier
Example:
11 PPR

Gear ratio: 1:30

Using both edges of A → multiplier = 2

Interrupts per full output shaft rotation
=
11
×
30
×
2
=
660
 interrupts
Interrupts per full output shaft rotation=11×30×2= 
660 interrupts
​
 
If using 4x decoding:

11
×
30
×
4
=
1320
 counts
11×30×4= 
1320 counts
​

*/