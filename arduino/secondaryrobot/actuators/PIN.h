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

#define LEFT_MOTOR_EN    6
#define LEFT_MOTOR_PWM   3
#define LEFT_MOTOR_DIR   5
#define RIGHT_MOTOR_EN   9
#define RIGHT_MOTOR_PWM 11
#define RIGHT_MOTOR_DIR 10
#define DRIVER_RESET    12


// Servos

#define DISPENSER_SERVO_PIN A4

#endif // __PIN_H__
