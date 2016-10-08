#ifndef __ROBOTCOM_H__
#define __ROBOTCOM_H__

#define ROBOTCOM_EMPTY_PIN 0
#define ROBOTCOM_BAUDRATE 115200

#define ROBOTCOM_COMMAND_START_BYTE byte('R')
#define ROBOTCOM_COMMAND_MAX_OPCODE 16  // at most 256
#define ROBOTCOM_COMMAND_MAX_LENGTH 32  // at most 256

#define ROBOTCOM_OUTPUT_START_BYTE  byte('A')
#define ROBOTCOM_OUTPUT_MAX_LENGTH  32  // at most 256

#define ROBOTCOM_UUID_LENGTH    9
#define ROBOTCOM_UUID_ADDRESS   0x0000

#define ROBOTCOM_GET_UUID_OPCODE 0x00
#define ROBOTCOM_SET_UUID_OPCODE 0x01

namespace RobotCom
{
typedef int (*Command)(int, byte[], byte[]);

void init(String uuid);
void init(void);

int send(int argc, byte argv[]);

void addCommand(byte opcode, Command command);
void executeCommands();

String getUUID();
String randomUUID();
void setUUID(String uuid);

}; // namespace RobotCom

#endif // __ROBOTCOM_H__