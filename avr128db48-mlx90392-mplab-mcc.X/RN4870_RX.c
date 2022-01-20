#include "RN4870_RX.h"

#include <avr/io.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "usart2.h"
#include "TCB0_oneShot.h"

//Modified by ISRs
volatile char RN4870RX_Buffer[RN4870_RX_BUFFER_SIZE];
volatile uint8_t writeIndex = 0;
volatile bool readReady = false, inStatus = false, cmdOccurred = false;

//NOT modified or accessed by ISRs
char responseBuffer[4];
uint8_t readIndex = 0;

//Initializes the RX Engine for the RN4870
void RN4870RX_init(void)
{
    RN4870RX_clearBuffer();
}

//Insert a character into the buffer
void RN4870RX_loadCharacter(char input)
{
    if (inStatus)
    {
        //Waiting for End of Status Marker
        //Don't Save Text
        
        if (input == RN4870_DELIM_STATUS)
        {
            inStatus = false;
        }
        
        return;
    }
    else
    {
        //Input is the deliminator of status
        if (input == RN4870_DELIM_STATUS)
        {
            inStatus = true;
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
    
    RN4870RX_Buffer[writeIndex] = input;
    writeIndex++;
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
    responseBuffer[3] = '\0';
    responseBuffer[2] = '\0';
    responseBuffer[1] = '\0';
    responseBuffer[0] = '\0';
}

//Load response buffer with the last 3 bytes received
void RN4870RX_loadResponseBuffer(void)
{
    //Clear Response Buffer
    RN4870RX_clearResponseBuffer();
    
    //Clear flag
    readReady = false;
    
    while ((RN4870RX_Buffer[readIndex] != RN4870_DELIM_RESP) && (readIndex != writeIndex))
    {
        responseBuffer[2] = responseBuffer[1];
        responseBuffer[1] = responseBuffer[0];
        responseBuffer[0] = RN4870RX_Buffer[readIndex];
        readIndex++;
    }
    
    //If we stopped because \r is the current char, clear the \r
    if (RN4870RX_Buffer[readIndex] == RN4870_DELIM_RESP)
    {
        RN4870RX_Buffer[readIndex] = '\0';
    }
    
    
}

//Discards the buffer
void RN4870RX_clearBuffer(void)
{
    RN4870RX_clearResponseBuffer();
    
    for (uint16_t i = 0; i < RN4870_RX_BUFFER_SIZE; i++)
    {
        RN4870RX_Buffer[i] = '\0';
    }
    
    writeIndex = 0;
    readIndex = 0;
    readReady = false;
    inStatus = false;
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
            if (strstr(&responseBuffer[0], compare) != 0)
            {
                asm("NOP");
                return true;
            }
            
            asm("NOP");
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
