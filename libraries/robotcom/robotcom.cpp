#include "Arduino.h"
#include "EEPROM.h"

#include "robotcom.h"

namespace RobotCom
{
// Global variables

static byte inputBuffer[ROBOTCOM_COMMAND_MAX_LENGTH + 2];
static int inputBufferLength = 0;

static Command commandsList[ROBOTCOM_COMMAND_MAX_OPCODE] = {NULL};

static byte outputBuffer[ROBOTCOM_OUTPUT_MAX_LENGTH];

// Built-in commands

static int getUUIDCommand(int argc, byte argv[], byte outv[])
{
    String uuid = getUUID();
    int uuidLength = uuid.length();
    for (int i = 0; i < uuidLength; i++)
        outv[i + 1] = byte(uuid.charAt(i));
    return uuidLength + 1;
}

static int setUUIDCommand(int argc, byte argv[], byte outv[])
{
    String uuid;
    uuid.reserve(argc - 1);
    for (int i = 1; i < argc; i++)
        uuid += char(argv[i]);
    setUUID(uuid);
    return 0;
}

// Library core

void init(String uuid)
{
    // Initialize serial port
    Serial.begin(ROBOTCOM_BAUDRATE);
    
    // Initialize UUID stuff
    if (getUUID() == "")
        setUUID(uuid);
    
    // Add UUID accessors
    addCommand(ROBOTCOM_UUID_GET_OPCODE, getUUIDCommand);
    addCommand(ROBOTCOM_UUID_SET_OPCODE, setUUIDCommand);
}

void init(void)
{
    init(randomUUID());
}

void addCommand(char opcode, Command command)
{
    // Add a command to execute when receiving the specified opcode
    if (opcode < ROBOTCOM_COMMAND_MAX_OPCODE)
        commandsList[opcode] = command;
}

void executeCommands()
{
    // Read incoming commands
    int serialLength = Serial.available();
    for (int i = 0; i < serialLength; i++)
    {
        // Append the incoming byte to the current command
        byte newByte = byte(Serial.read());
        if (newByte == ROBOTCOM_COMMAND_START_BYTE || inputBufferLength > 0)
            inputBuffer[inputBufferLength++] = newByte;
        
        // Execute it if this is the end of the current command
        if (inputBufferLength >= 2)
        {
            byte* commandBuffer = inputBuffer + 2;
            int commandLength = inputBuffer[1];
            if (commandLength <= inputBufferLength - 2)
            {
                if (commandLength > 0)
                {
                    byte opcode = commandBuffer[0];
                    int outputLength = 0;
                    outputBuffer[0] = opcode;
                    if (commandsList[opcode] != NULL)
                        outputLength = commandsList[opcode](commandLength, commandBuffer, outputBuffer);
                    if (outputLength > 0)
                    {
                        Serial.write(ROBOTCOM_OUTPUT_START_BYTE);
                        Serial.write(outputLength);
                        Serial.write(outputBuffer, outputLength);
                    }
                }
                inputBufferLength = 0;
            }
        }
    }
}

String getUUID()
{
    String uuid = "";
    for (int i = 0; i < EEPROM.length(); i++)
    {
        char ch = EEPROM.read(ROBOTCOM_UUID_ADDRESS + i);
        if (ch != '\0' && ch != 0xFF)
            uuid += ch;
        else
            return uuid;
    }
    return "";
}

String randomUUID()
{
    // Initialize the random number generator
    randomSeed(analogRead(ROBOTCOM_EMPTY_PIN));
    
    // Generate the UUID from a list of random hexadecimal numbers
    String uuid = "";
    for (int i = 0; i < ROBOTCOM_UUID_LENGTH; i++)
    {
        if (i % 5 == 4)
            uuid += '-';
        else
            uuid += String(random(16), HEX);
    }
    return uuid;
}

void setUUID(String uuid)
{
    int uuidLength = uuid.length();
    for (int i = 0; i < uuidLength; i++)
    {
        char ch = uuid.charAt(i);
        EEPROM.write(ROBOTCOM_UUID_ADDRESS + i, ch);
    }
    EEPROM.write(ROBOTCOM_UUID_ADDRESS + uuidLength, '\0');
}

} // namespace RobotCom
