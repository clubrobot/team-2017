#include <Arduino.h>
#include "PIN.h"
#include "../common/SoftwareSerial.h"
#include "../common/AX12.h"
#include "../common/SerialTalks.h"

SoftwareSerial SoftSerial(RX, TX);

AX12 servoax;

DCMotorsDriver motorDriver;

DCMotor rollerMotor;
DCMotor hammerMotor;

void setup(){
  Serial.begin(SERIALTALKS_BAUDRATE);
  talks.begin(Serial);
  talks.bind(_SET_ROLLER_VELOCITY_OPCODE, SET_ROLLER_VELOCITY);
  talks.bind(_SET_FIRING_HAMMER_VELOCITY_OPCODE,  SET_FIRING_HAMMER_VELOCITY);
  talks.bind(_SET_AX_POSITION_OPCODE, SET_AX_POSITION);
  talks.bind(_GET_AX_TORQUE_OPCODE, GET_AX_TORQUE);
  talks.bind(_SET_AX_VELOCITY_MOVE_OPCODE,  SET_AX_VELOCITY_MOVE);

  AX12::SerialBegin(9600,RX,TX,DATA_CONTROL);  
  servoax.attach(2);
  servoax.setEndlessMode(OFF);

  motorDriver.attach(DRIVER_RESET , A7);
  motorDriver.reset();
    
  rollerMotor.attach(MOTOR1_EN, MOTOR1_PWM, MOTOR1_DIR);
  rollerMotor.setConstant(1/11.1);

  hammerMotor.attach(MOTOR2_EN, MOTOR2_PWM, MOTOR2_DIR);
  hammerMotor.setConstant(1/11.1);
  
   // Miscellanous
	TCCR2B = (TCCR2B & 0b11111000) | 1;
}

void loop(){
  talks.execute();
}