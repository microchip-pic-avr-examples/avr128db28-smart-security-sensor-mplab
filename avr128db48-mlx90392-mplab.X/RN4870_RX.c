#include "RN4870_RX.h"

#include <avr/io.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "usart0.h"
#include "TCB0_oneShot.h"
#include "GPIO.h"
#include "ringBuffer.h"

volatile static bool processingMessage = false, isMessageComplete = false;
static char cMemory[RN4870_RX_BUFFER_SIZE];
static RingBuffer ringBuffer;

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
    ringBuffer_createBuffer(&ringBuffer, &cMemory[0], RN4870_RX_BUFFER_SIZE);
}

//Insert a character into the buffer
void RN4870RX_loadCharacter(char input)
{
    //Do not allow insertions of the protected character
    if (input == RN4870_DELIM_RING_BUFFER)
    {
        return;
    }
    
    if (processingMessage)
    {
        //Waiting for End of Status Marker
        
        if ((input == RN4870_DELIM_STATUS) || (input == RN4870_DELIM_USER) || (input == '\r') || (input == '\n'))
        {
            ringBuffer_loadCharacter(&ringBuffer, RN4870_DELIM_RING_BUFFER);
            
            isMessageComplete = true;
            processingMessage = false;
        }
        else if (input != ' ')
        {
            ringBuffer_loadCharacter(&ringBuffer, convertToUppercase(input));
        }
        
        return;
    }
    else
    {
        //Input is the deliminator of status
        if ((input == RN4870_DELIM_STATUS) || (input == RN4870_DELIM_USER))
        {
            processingMessage = true;
        }
        ringBuffer_loadCharacter(&ringBuffer, input);
    }  
}

//Returns true if status / user commands are empty
bool RN4870RX_isEmpty(void)
{
    //No Chars
    if (ringBuffer_isEmpty(&ringBuffer))
        return true;
    
    if (isMessageComplete)
    {
        return false;
    }
    
    return true;
}        

//Returns true if a status message (%TEXT%) was received
bool RN4870RX_isStatusRX(void)
{  
    return (ringBuffer_peekChar(&ringBuffer) == RN4870_DELIM_STATUS);
}

//Returns true if a user command (!TEXT!) was received
bool RN4870RX_isUserRX(void)
{   
    return (ringBuffer_peekChar(&ringBuffer) == RN4870_DELIM_USER);
}

void RN4870RX_clearStatusRX(void)
{
    ringBuffer_flushReadBuffer(&ringBuffer);
}

//Returns true if status matches COMP string
bool RN4870RX_find(const char* comp)
{  
    return ringBuffer_find(&ringBuffer, comp);
}

//Advances to the next status / command in the buffer, if available.
void RN4870RX_advanceMessage(void)
{
    ringBuffer_advanceToString(&ringBuffer, "#");
    
    //No other messages
    if (ringBuffer_isEmpty(&ringBuffer))
    {
        isMessageComplete = false;
    }
}

//Fills a buffer with a copy of the current message
void RN4870RX_copyMessage(char* buffer, uint8_t size)
{
    //If nothing to copy
    ringBuffer_copyBufferUntil(&ringBuffer, buffer, RN4870_DELIM_RING_BUFFER, size);
}

//Copies the parameter of the command.
//Returns false if no parameter is present
bool RN4870RX_copyMessageParameter(char* buffer, uint8_t size)
{
    if (RN4870RX_isEmpty())
        return false;
    
    //Copy upto the next message
    if (ringBuffer_copyAndChop(&ringBuffer, buffer, ',', RN4870_DELIM_RING_BUFFER, size) == 1)
        return false;
    
    return true;
}

//Returns true if RESP_DELIM was the last character received.
bool RN4870RX_isResponseComplete(void)
{
    return isMessageComplete;
}

//Discards the buffer
void RN4870RX_clearBuffer(void)
{
    ringBuffer_flushReadBuffer(&ringBuffer);
    processingMessage = false;
    isMessageComplete = false;
}

//Waits for a message to be received and checks to see if it matches string.
//Timeout is specified in milliseconds
bool RN4870RX_waitForResponseRX(uint16_t timeout, const char* compare)
{
    uint16_t timeCycles = 0;
    ringBuffer_flushReadBuffer(&ringBuffer);
    
    do
    {
        if (ringBuffer_find(&ringBuffer, compare))
        {                       
            return true;
        }
        
        if (!TCB0_isRunning())
        {
            TCB0_triggerTimer();
            timeCycles++;
        }
        
    } while (timeout > timeCycles);
    
    asm("NOP");
    return false;
}

bool RN4870RX_waitForCommandRX(uint16_t timeout)
{
    uint16_t timeCycles = 0;
    ringBuffer_flushReadBuffer(&ringBuffer);
    
    do
    {
        if (ringBuffer_charsToRead(&ringBuffer) >= 5)
        {
            //Advance to the next element, if possible
            //Returns false if not present
            return ringBuffer_advanceToString(&ringBuffer, "> ");
        }
        
        if (!TCB0_isRunning())
        {
            TCB0_triggerTimer();
            timeCycles++;
        }
        
    } while (timeout > timeCycles);
    
    return false;
}