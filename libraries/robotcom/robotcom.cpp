#include "Arduino.h"
#include "EEPROM.h"

#include "robotcom.h"

String RobotCom::buffer = "";
RobotCom::Command RobotCom::commands[ROBOTCOM_COMMAND_MAX_OPCODE] = {NULL};

void RobotCom::init()
{
    // Initialize serial port
    Serial.begin(ROBOTCOM_BAUD_RATE);

    // Initialize UUID stuff
    if (getUUID() == "")
        generateUUID();
}

void RobotCom::addCommand(char opcode, RobotCom::Command command)
{
    // Add a command to execute when receiving the specified opcode
    if (opcode < ROBOTCOM_COMMAND_MAX_OPCODE)
        commands[opcode] = command;
}

void RobotCom::executeCommands()
{
    // Read incoming commands
    int inLen = Serial.available();
    for (int i = 0; i < inLen; i++)
    {
        // Append the incoming byte to the current command
        byte in = byte(Serial.read());
        if (in == ROBOTCOM_COMMAND_START || buffer.length() > 0)
            buffer += char(in);
        
        // Execute it if this is the end of the current command
        if (buffer.length() >= 2)
        {
            int cmdLen = byte(buffer.charAt(1));
            if (cmdLen <= buffer.length() - 2)
            {
                if (buffer.length() > 2)
                {
                    byte opcode = byte(buffer.charAt(2));
                    if (commands[opcode] != NULL)
                        commands[opcode](buffer.substring(3, cmdLen + 2));                 
                }
                buffer = "";
            }
        }
    }
}

String RobotCom::getUUID()
{
    // Reserve enough space for containing the whole UUID
    String uuid;
    uuid.reserve(ROBOTCOM_UUID_LENGTH);

    // Read the UUID from EEPROM
    for (int i = 0; i < ROBOTCOM_UUID_LENGTH; i++)
    {
        byte ch = EEPROM.read(ROBOTCOM_UUID_EEPROM + i);
        if (ch == 0xFF)
            return "";
        else
            uuid.setCharAt(i, char(ch));
    }
    return uuid;
}

void RobotCom::generateUUID()
{
    // Initialize the random number generator
    randomSeed(analogRead(ROBOTCOM_EMPTY_PIN));
    
    // Generate the UUID from a list of random hexadecimal numbers
    for (int i = 0; i < ROBOTCOM_UUID_LENGTH; i++)
    {
        byte ch;
        if (i % 5 == 4)
            ch = byte('-');
        else
        {
            ch = random(ROBOTCOM_UUID_MAX_DIGIT);
            if (ch < 10)
                ch += byte('0');
            else
                ch += byte('a');
        }
        EEPROM.write(ROBOTCOM_UUID_EEPROM + i, ch);
    }
}