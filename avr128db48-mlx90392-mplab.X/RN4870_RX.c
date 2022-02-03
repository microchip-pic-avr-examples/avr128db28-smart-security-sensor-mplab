#include "RN4870_RX.h"

#include <avr/io.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "usart2.h"
#include "TCB0_oneShot.h"
#include "GPIO.h"

//Modified by ISRs
volatile static char RN4870RX_buffer[RN4870_RX_BUFFER_SIZE];
volatile static char RN4870RX_statusBuffer[RN4870_RX_STATUS_BUFFER_SIZE];
volatile static char* respPosition = &RN4870RX_statusBuffer[0];
volatile static char currentDelim = '\0';
volatile static uint8_t writeIndex = 0, respWriteIndex = 0, respReadIndex = 0, respCount = 0;
volatile static bool readReady = false, processingMessage = false, cmdOccurred = false;

//NOT modified or accessed by ISRs
static char RN4870RX_responseBuffer[4];
static uint8_t readIndex = 0;

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
    RN4870RX_clearBuffer();
}

//Insert a character into the buffer
void RN4870RX_loadCharacter(char input)
{
    DBG_OUT_Toggle();
    if (processingMessage)
    {
        //Waiting for End of Status Marker
        
        if ((input == currentDelim) || (input == '\r'))
        {
            RN4870RX_statusBuffer[respWriteIndex] = '\0';
            
            processingMessage = false;
            respCount++;
        }
        else
        {
            RN4870RX_statusBuffer[respWriteIndex] = convertToUppercase(input);
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
            //Store the Deliminator
            RN4870RX_statusBuffer[respWriteIndex] = input;
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
    
    RN4870RX_buffer[writeIndex] = input;
    writeIndex++;
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
    
    return (respPosition[0] == RN4870_DELIM_STATUS);
}

//Returns true if a user command (!TEXT!) was received
bool RN4870RX_isUserRX(void)
{
    if (respCount == 0)
        return false;
    
    return (respPosition[0] == RN4870_DELIM_USER);
}

void RN4870RX_clearStatusRX(void)
{
    respCount = 0;
    respReadIndex = 0;
    respWriteIndex = 0;
    respPosition = &RN4870RX_statusBuffer[0];
}

//Returns true if status matches COMP string
bool RN4870RX_searchMessage(const char* comp)
{
    //Compare strings
    if (strstr(respPosition, comp) != 0)
        return true;
    
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
            readIndex++;
        }
        while ((RN4870RX_statusBuffer[readIndex] != RN4870_DELIM_RESP) && (RN4870RX_statusBuffer[readIndex] != RN4870_DELIM_USER));
        
        respPosition = &RN4870RX_statusBuffer[readIndex];
    }
    
    
}

//Returns the message
const char* RN4870RX_getMessageBuffer(void)
{
    return &respPosition[0];
}

//Returns the substring after the ','. Returns null if not present
char* RN4870RX_getMessageParameter(void)
{
    char* ptr = strstr(respPosition, ",");
    
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
    RN4870RX_responseBuffer[3] = '\0';
    RN4870RX_responseBuffer[2] = '\0';
    RN4870RX_responseBuffer[1] = '\0';
    RN4870RX_responseBuffer[0] = '\0';
}

//Load response buffer with the last 3 bytes received
void RN4870RX_loadResponseBuffer(void)
{
    //Clear Response Buffer
    RN4870RX_clearResponseBuffer();
    
    //Clear flag
    readReady = false;
    
    while ((RN4870RX_buffer[readIndex] != RN4870_DELIM_RESP) && (readIndex != writeIndex))
    {
        RN4870RX_responseBuffer[2] = RN4870RX_responseBuffer[1];
        RN4870RX_responseBuffer[1] = RN4870RX_responseBuffer[0];
        RN4870RX_responseBuffer[0] = RN4870RX_buffer[readIndex];
        readIndex++;
    }
    
    //If we stopped because \r is the current char, clear the \r
    if (RN4870RX_buffer[readIndex] == RN4870_DELIM_RESP)
    {
        RN4870RX_buffer[readIndex] = '\0';
    }
    
    
}

//Discards the buffer
void RN4870RX_clearBuffer(void)
{
    RN4870RX_clearResponseBuffer();
        
    //Main Buffer
    for (uint16_t i = 0; i < RN4870_RX_BUFFER_SIZE; i++)
    {
        RN4870RX_buffer[i] = '\0';
    }
    
    //Indexes
    respWriteIndex = 0;
    writeIndex = 0;
    readIndex = 0;
    
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
            if (strstr(&RN4870RX_responseBuffer[0], compare) != 0)
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
