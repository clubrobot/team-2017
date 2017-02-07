#ifndef __PIN_H__
#define __PIN_H__

#include <Arduino.h>

/*
Pin arduino carte actionneurs

-----------------------------------------
INTER	1	2	3	4	5
pin	2	4	7	8	13
-----------------------------------------
Driver moteur		Pin
PWM_moteur_1		3
Select_moteur_1		5
EN_moteur_1		6
PWM_moteur_2		11
Select_moteur_2		10
EN_moteur_2		9
Reset_driver		1
-----------------------------------------
SERVO	1	2	3	4
pin	A5	A4	A3	A2
*/

// DC motors driver

#define MOTOR1_EN    9
#define MOTOR1_PWM   11
#define MOTOR1_DIR   10
#define MOTOR2_EN   6
#define MOTOR2_PWM 3
#define MOTOR2_DIR 5
#define DRIVER_RESET    12

// Switchs

#define SWITCH1 2
#define SWITCH2 4
#define SWITCH3 7
#define SWITCH4 8
#define SWITCH5 13

// Servos

#define DISPENSER_SERVO_PIN A4
#define SERVO2 A5
#define SERVO3 A3
#define SERVO4 A2

#endif // __PIN_H__
