#include <Arduino.h>
#include "PIN.h"
#include "../common/SoftwareSerial.h"
#include "../common/AX12.h"
#include "../common/SerialTalks.h"
#include "../common/DCMotor.h"
#include "instructions.h"
#include "../common/PeriodicProcess.h"

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
  talks.bind(_PING_AX_OPCODE, PING_AX);
  talks.bind(_SET_AX_HOLD_OPCODE, SET_AX_HOLD);
  talks.bind(_GET_AX_POSITION_OPCODE, GET_AX_POSITION);
  talks.bind(AX12_SEND_INSTRUCTION_PACKET_OPCODE, AX12_SEND_INSTRUCTION_PACKET);
  talks.bind(AX12_RECEIVE_STATUS_PACKET_OPCODE, AX12_RECEIVE_STATUS_PACKET);

  AX12::SerialBegin(9600,RX,TX,DATA_CONTROL);

  motorDriver.attach(DRIVER_RESET , A7);
  motorDriver.reset();
    
  rollerMotor.attach(MOTOR2_EN, MOTOR2_PWM, MOTOR2_DIR);
  rollerMotor.setConstant(1/11.1);

  hammerMotor.attach(MOTOR1_EN, MOTOR1_PWM, MOTOR1_DIR);
  hammerMotor.setConstant(1/11.1);

   // Miscellanous
	//TCCR2B = (TCCR2B & 0b11111000) | 1;

  servoax.attach(2);
  servoax.setSRL(2); // Respond only to READ_DATA instructions
  servoax.setLEDAlarm(32); // max torque only
  servoax.setShutdownAlarm(32); // max torque only
  servoax.setMaxTorque(1023);
  servoax.setEndlessMode(OFF);
  servoax.hold(OFF);
}

void loop(){
  talks.execute();
}