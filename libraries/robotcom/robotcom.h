#ifndef __ROBOTCOM_H__
#define __ROBOTCOM_H__

#define ROBOTCOM_EMPTY_PIN 0
#define ROBOTCOM_BAUD_RATE 115200

#define ROBOTCOM_COMMAND_START 0x52
#define ROBOTCOM_COMMAND_MAX_OPCODE 256

#define ROBOTCOM_UUID_MAX_DIGIT 16
#define ROBOTCOM_UUID_LENGTH 9
#define ROBOTCOM_UUID_EEPROM 0x0000

class RobotCom
{
    typedef String (*Command)(String args);

public:
    static void init();
    static void addCommand(char opcode, Command cmd);
    static void executeCommands();
    static String getUUID();
    
private:
    static void generateUUID();

    static String buffer;
    static Command commands[ROBOTCOM_COMMAND_MAX_OPCODE];
};

#endif // __ROBOTCOM_H__