
BTS7960 Pin	nRF5340 Pin	Function
RPWM	P0.18	Motor PWM (Right)
LPWM	P0.19	Motor PWM (Left)
R_EN	P0.16	Enable Right Channel
L_EN	P0.17	Enable Left Channel
R_IS	NC	Not Used (Overcurrent Protection)
L_IS	NC	Not Used (Overcurrent Protection)
VCC	3.3V	Logic Power
GND	GND	Ground
M+	Motor Terminal 1	Motor Output
M-	Motor Terminal 2	Motor Output
B+	Battery Positive (12V or 24V)	Motor Power
B-	Battery Ground	Motor Power
 Functionality:

RPWM and LPWM are controlled using PWM signals to adjust speed and direction.
R_EN and L_EN must be set HIGH to enable movement.
BTS7960 provides high-power motor control (supports up to 43A).


Encoder Pin	nRF5340 Pin	Function
Encoder PWR+	3.3V	Encoder Power
Encoder PWR-	GND	Ground
MOTOR PWR+	BTS7960 M+	Motor Terminal 1
MOTOR PWR-	BTS7960 M-	Motor Terminal 2
OUT C1/A	P0.23	Quadrature Encoder Signal A
OUT C2/B	P0.24	Quadrature Encoder Signal B
✅ Functionality:

OUT C1/A and OUT C2/B provide quadrature signals to track motor position & speed.
These pins are read using interrupts or timers in nRF5340.