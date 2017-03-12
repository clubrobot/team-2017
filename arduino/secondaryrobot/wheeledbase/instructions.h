#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__

#include "../../common/SerialTalks.h"

// Opcodes declaration

#define SET_OPENLOOP_VELOCITIES_OPCODE   0x04

#define GET_CODEWHEELS_COUNTERS_OPCODE   0x0D

#define SET_VELOCITIES_OPCODE    0x06

#define START_TRAJECTORY_OPCODE  0x07
#define TRAJECTORY_ENDED_OPCODE  0x08

#define SET_POSITION_OPCODE      0x0A
#define GET_POSITION_OPCODE      0x0B
#define GET_VELOCITIES_OPCODE    0x0C

#define SET_PARAMETER_VALUE_OPCODE   0x0E
#define GET_PARAMETER_VALUE_OPCODE   0x0F

// Parameters identifiers

#define LEFTWHEEL_RADIUS_ID          0x10
#define LEFTWHEEL_CONSTANT_ID        0x11
#define RIGHTWHEEL_RADIUS_ID         0x20
#define RIGHTWHEEL_CONSTANT_ID       0x21
#define LEFTCODEWHEEL_RADIUS_ID        0x40
#define LEFTCODEWHEEL_COUNTSPERREV_ID  0x41
#define RIGHTCODEWHEEL_RADIUS_ID       0x50
#define RIGHTCODEWHEEL_COUNTSPERREV_ID 0x51
#define ODOMETRY_AXLETRACK_ID        0x60
#define VELOCITYCONTROL_AXLETRACK_ID 0x80
#define VELOCITYCONTROL_MAXLINACC_ID 0x81
#define VELOCITYCONTROL_MAXLINDEC_ID 0x82
#define VELOCITYCONTROL_MAXANGACC_ID 0x83
#define VELOCITYCONTROL_MAXANGDEC_ID 0x84
#define LINVELPID_KP_ID              0xA0
#define LINVELPID_KI_ID              0xA1
#define LINVELPID_KD_ID              0xA2
#define LINVELPID_MINOUTPUT_ID       0xA3
#define LINVELPID_MAXOUTPUT_ID       0xA4
#define ANGVELPID_KP_ID              0xB0
#define ANGVELPID_KI_ID              0xB1
#define ANGVELPID_KD_ID              0xB2
#define ANGVELPID_MINOUTPUT_ID       0xB3
#define ANGVELPID_MAXOUTPUT_ID       0xB4
#define TRAJECTORY_LINVELKP_ID       0xD0
#define TRAJECTORY_ANGVELKP_ID       0xD1
#define TRAJECTORY_LINVELMAX_ID      0xD2
#define TRAJECTORY_ANGVELMAX_ID      0xD3
#define TRAJECTORY_LOOKAHED_ID       0xD4

// Instructions prototypes

void SET_OPENLOOP_VELOCITIES(SerialTalks& talks, Deserializer& input, Serializer& output);

void GET_CODEWHEELS_COUNTERS(SerialTalks& talks, Deserializer& input, Serializer& output);

void SET_VELOCITIES(SerialTalks& talks, Deserializer& input, Serializer& output);

void START_TRAJECTORY(SerialTalks& talks, Deserializer& input, Serializer& output);

void TRAJECTORY_ENDED(SerialTalks& talks, Deserializer& input, Serializer& output);

void SET_POSITION(SerialTalks& talks, Deserializer& input, Serializer& output);

void GET_POSITION(SerialTalks& talks, Deserializer& input, Serializer& output);

void GET_VELOCITIES(SerialTalks& talks, Deserializer& input, Serializer& output);

void SET_PARAMETER_VALUE(SerialTalks& talks, Deserializer& input, Serializer& output);

void GET_PARAMETER_VALUE(SerialTalks& talks, Deserializer& input, Serializer& output);

#endif // __INSTRUCTIONS_H__
