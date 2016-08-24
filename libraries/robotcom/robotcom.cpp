#include "Arduino.h"
#include "EEPROM.h"

#include "robotcom.h"

void RobotCom::init()
{
    // Initialize serial port
    Serial.begin(ROBOTCOM_BAUDRATE);
    while (!Serial);

    // Initialize commands buffer
    buffer = "";

    // Initialize commands list
    for (char opcode = 0x00; opcode < ROBOTCOM_MAX_OPCODE; opcode++)
        commands[opcode] = NULL;

    // Initialize UUID stuff
    loadUUID()
    if (getUUID() == NULL)
    {
        generateUUID();
        saveUUID();
    }
}

void RobotCom::addCommand(char opcode, Command cmd)
{
    // Add a command to execute when receiving the specified opcode
    if (opcode < ROBOTCOM_COMMAND_MAX_OPCODE)
        RobotCom::commands[opcode] = cmd;
}

void RobotCom::executeCommands()
{
    // Read incoming commands
    int serialLength = Serial.available();
    for (int i = 0; i < serialLength; i++)
    {
        // Append the incoming byte to the current command
        char incoming = char(Serial.read());
        if (incoming == ROBOTCOM_COMMAND_START || buffer.length() > 0)
            buffer += incoming;
        
        // Execute it if this is the end of the current command
        if (buffer.length() >= 2)
        {
            int commandLength = byte(buffer.charAt(1));
            if (commandLength <= buffer.length() - 2)
            {
                if (buffer.length() > 2)
                {
                    byte opcode = byte(buffer.charAt(2));
                    if (commands[opcode] != NULL)
                        commands[opcode](2, commandLength + 2);                 
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
        byte c = EEPROM.read(ROBOTCOM_UUID_EEPROM + i);
        if (c == 0xFF)
            return uuid;
        else
            uuid.setCharAt(i, char(c));
    }
    return uuid;
}

void RobotCom::generateUUID()
{
    // Initialize the random number generator
    randomSeed(analogRead(ROBOTCOM_EMPTY_PIN));
    
    // Generate the UUID from a list of random numbers
    for (int i = 0; i < ROBOTCOM_UUID_LENGTH; i++)
    {
        byte c;
        if (i % 5 == 4)
            c = byte('-');
        else
        {
            c = random(ROBOTCOM_UUID_MAX_NUMBER);
            if (c < 10)
                c += byte('0');
            else
                c += byte('a');
        }
        EEPROM.write(ROBOTCOM_UUID_EEPROM + i, c);
    }
}