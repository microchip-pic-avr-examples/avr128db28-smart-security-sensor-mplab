#include "RN4870_RX.h"

#include <avr/io.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "usart2.h"
#include "TCB0_oneShot.h"
#include "GPIO.h"

//Modified by ISRs
volatile static char generalTextBuffer[RN4870_RX_BUFFER_SIZE];
volatile static char statusCommandBuffer[RN4870_RX_STATUS_BUFFER_SIZE + 1];
volatile static char* statusCommandPosition = &statusCommandBuffer[0];
volatile static char currentDelim = '\0';
volatile static uint8_t commandWriteIndex = 0, respWriteIndex = 0, respReadIndex = 0, respCount = 0;
volatile static bool readReady = false, processingMessage = false, cmdOccurred = false;

//NOT modified or accessed by ISRs
static char commandResponseBuffer[4];
static uint8_t commandReadIndex = 0;

//Utility Function - Converts input to uppercase
char convertToUppercase(char in) { 
    if ((in >= 'a') && (in <= 'z')) 
    { 
        in = (in - 'a') + 'A';
    }
    return in;
}

//Initializes the RX Engine for the RN4870
void RN4870RX_init(void)
{
    //Protective Terminator
    statusCommandBuffer[RN4870_RX_BUFFER_SIZE] = '\0';

    //Reset Buffers
    RN4870RX_clearBuffer();
}

//Insert a character into the buffer
void RN4870RX_loadCharacter(char input)
{
    if (processingMessage)
    {
        //Waiting for End of Status Marker
        
        if ((input == currentDelim) || (input == '\r') || (input == '\n'))
        {
            DBG_OUT_SetLow();
            statusCommandBuffer[respWriteIndex] = '\0';
            
            processingMessage = false;
            respCount++;
        }
        else if (input != ' ')
        {
            statusCommandBuffer[respWriteIndex] = convertToUppercase(input);
        }
        
        //Increment writeIndex for response
        respWriteIndex++;
        
        return;
    }
    else
    {
        //Input is the deliminator of status
        if ((input == RN4870_DELIM_STATUS) || (input == RN4870_DELIM_USER))
        {
            DBG_OUT_SetHigh();
            //Store the Deliminator
            statusCommandBuffer[respWriteIndex] = input;
            respWriteIndex++;
            
            currentDelim = input;
            processingMessage = true;
            return;
        }
        
        if (input == RN4870_DELIM_RESP)
        {
            //If this is a deliminator for response messages "AOK", "ERR"
            readReady = true;
        }
    }  
    
    //If one of the chars from "CMD> " has been detected
    if (input == RN4870_MARKER_CMD)
    {
        cmdOccurred = true;
    }
    
    generalTextBuffer[commandWriteIndex] = input;
    commandWriteIndex++;
}

//Returns true if status / user commands are empty
bool RN4870RX_isEmpty(void)
{
    return (respCount == 0);
}        

//Returns true if a status message (%TEXT%) was received
bool RN4870RX_isStatusRX(void)
{  
    if (respCount == 0)
        return false;
    
    return (statusCommandPosition[0] == RN4870_DELIM_STATUS);
}

//Returns true if a user command (!TEXT!) was received
bool RN4870RX_isUserRX(void)
{
    if (respCount == 0)
        return false;
    
    return (statusCommandPosition[0] == RN4870_DELIM_USER);
}

void RN4870RX_clearStatusRX(void)
{
    respCount = 0;
    respReadIndex = 0;
    respWriteIndex = 0;
    statusCommandPosition = &statusCommandBuffer[0];
}

//Returns true if status matches COMP string
bool RN4870RX_searchMessage(const char* comp)
{
    volatile uint8_t rC = respCount;
    //Nothing to search
    if ((comp[0] == '\0') || (respCount == 0))
        return false;
    
    //Compare strings
    uint8_t tIndex = respReadIndex;
    uint8_t compIndex = 0;
    
    while (statusCommandBuffer[tIndex] != '\0')
    {
        if (comp[compIndex] == statusCommandBuffer[tIndex])
        {
            compIndex++;
        }
        else
        {
            compIndex = 0;
        }
        
        if (comp[compIndex] == '\0')
        {
            //End of String
            return true;
        }
        
        tIndex++;
    }
    
//    if (strstr(statusCommandPosition, comp) != 0)
//        return true;
    
    return false;
}

//Advances to the next status / command in the buffer, if available.
void RN4870RX_advanceMessage(void)
{
    //No new responses
    if (respCount == 0)
        return;
        
    //Decrement Counter
    respCount--;
    
    if (respCount == 0)
    {
        //If everything is 0, then flush the buffers
        RN4870RX_clearStatusRX();
    }
    else
    {
        //Find the start of the next message
        
        //Move to the end of the message (NULL)
        do
        {
            respReadIndex++;
        }
        while ((statusCommandBuffer[respReadIndex] != RN4870_DELIM_RESP) && (statusCommandBuffer[respReadIndex] != RN4870_DELIM_USER));
        
        statusCommandPosition = &statusCommandBuffer[respReadIndex];
    }
    
    
}

//Returns the message
const char* RN4870RX_getMessageBuffer(void)
{
    return &statusCommandPosition[0];
}

//Returns the substring after the ','. Returns null if not present
char* RN4870RX_getMessageParameter(void)
{
    char* ptr = strstr(statusCommandPosition, ",");
    
    //If there are no commas, return NULL
    if (ptr == NULL)
    {
        return NULL;
    }
    
    return &ptr[1];
}

void RN4870RX_clearCMDFlag(void)
{
    cmdOccurred = false;
}

//Returns true if RESP_DELIM was the last character received.
bool RN4870RX_isResponseComplete(void)
{
    return readReady;
}

bool RN4870RX_isCMDPresent(void)
{
    return cmdOccurred;
}

//Clears the response buffer
void RN4870RX_clearResponseBuffer(void)
{
    //Clear Response Buffer
    commandResponseBuffer[3] = '\0';
    commandResponseBuffer[2] = '\0';
    commandResponseBuffer[1] = '\0';
    commandResponseBuffer[0] = '\0';
}

//Load response buffer with the last 3 bytes received
void RN4870RX_loadResponseBuffer(void)
{
    //Clear Response Buffer
    RN4870RX_clearResponseBuffer();
    
    //Clear flag
    readReady = false;
    
    while ((generalTextBuffer[commandReadIndex] != RN4870_DELIM_RESP) && (commandReadIndex != commandWriteIndex))
    {
        commandResponseBuffer[2] = commandResponseBuffer[1];
        commandResponseBuffer[1] = commandResponseBuffer[0];
        commandResponseBuffer[0] = generalTextBuffer[commandReadIndex];
        commandReadIndex++;
    }
    
    //If we stopped because \r is the current char, clear the \r
    if (generalTextBuffer[commandReadIndex] == RN4870_DELIM_RESP)
    {
        generalTextBuffer[commandReadIndex] = '\0';
    }
    
    
}

//Discards the buffer
void RN4870RX_clearBuffer(void)
{
    RN4870RX_clearResponseBuffer();
        
    //Main Buffer
    for (uint16_t i = 0; i < RN4870_RX_BUFFER_SIZE; i++)
    {
        generalTextBuffer[i] = '\0';
    }
    
    //Indexes
    respWriteIndex = 0;
    commandWriteIndex = 0;
    commandReadIndex = 0;
    
    //Statuses
    readReady = false;
    processingMessage = false;
    cmdOccurred = false;
}

//Waits for a message to be received and checks to see if it matches string.
//Timeout is specified in milliseconds
bool RN4870RX_waitForResponseRX(uint16_t timeout, const char* compare)
{
    uint16_t timeCycles = 0;
    
    do
    {
        if (RN4870RX_isResponseComplete())
        {
            //Load Response Buffer
            RN4870RX_loadResponseBuffer();
            
            //Compare strings
            if (strstr(&commandResponseBuffer[0], compare) != 0)
            {
                return true;
            }
            
            return false;
        }
        
        if (!TCB0_isRunning())
        {
            TCB0_triggerTimer();
            timeCycles++;
        }
        
    } while (timeout > timeCycles);
    
    return false;
}

bool RN4870RX_waitForCommandRX(uint16_t timeout)
{
    uint16_t timeCycles = 0;
    
    do
    {
        if (RN4870RX_isCMDPresent())
        {
            //Load Response Buffer
            RN4870RX_clearCMDFlag();
            return true;
        }
        
        if (!TCB0_isRunning())
        {
            TCB0_triggerTimer();
            timeCycles++;
        }
        
    } while (timeout > timeCycles);
    
    return false;
}
