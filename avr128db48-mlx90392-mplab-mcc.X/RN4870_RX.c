#include "RN4870_RX.h"
#include "mcc_generated_files/timer/tcb0.h"

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

volatile char RN4870RX_Buffer[RN4870_RX_BUFFER_SIZE];
volatile uint8_t writeIndex = 0, readIndex = 0;
volatile bool lastDelim = false, inStatus = false;

//Initializes the RX Engine for the RN4870
void RN4870RX_init(void)
{
    
}

ISR(USART2_RXC_vect)
{
    
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
            lastDelim = true;
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
            lastDelim = true;
            return;
        }
        else
        {
            //Normal Character
            lastDelim = false;
        }
    }
    
    
    RN4870RX_Buffer[writeIndex] = input;
    writeIndex++;
}

//Returns true if RESP_DELIM was the last character received.
bool RN4870X_isDataComplete(void)
{
    return lastDelim;
}

//Get the number of remaining characters to read
uint8_t RN4870RX_remaining(void)
{
    if (readIndex >= writeIndex)
    {
        RN4870RX_clearBuffer();
        return 0;
    }
    
    return (writeIndex - readIndex);
}

//Get the next character in the queue
char RN4870RX_getCharacter(void)
{
    if (readIndex >= writeIndex)
    {
        RN4870RX_clearBuffer();
        return '\0';
    }
    
    char c = RN4870RX_Buffer[readIndex];
    readIndex++;
    
    return c;
}

//Discards the buffer
void RN4870RX_clearBuffer(void)
{
    writeIndex = 0;
    readIndex = 0;
    lastDelim = false;
    inStatus = false;
}

//Waits for a message to be received and checks to see if it matches string.
//Timeout is specified in milliseconds
bool RN4870RX_waitForRX(uint16_t timeout, const char* compare)
{
    uint16_t timeCycles = 0;
    
    do
    {
        if (RN4870X_isDataComplete())
        {
            //Check Data
            uint8_t cIndex = 0;
            while ((compare[cIndex] != '\0') && (RN4870RX_remaining() != 0))
            {
                //Compare Characters
                if (compare[cIndex] != RN4870RX_getCharacter())
                {
                    //Match Failed
                    return false;
                }
                cIndex++;
            }
            
            return true;
        }
        
        if (!(TCB0.STATUS & TCB_RUN_bm))
        {
            //Ensures timing requirements
            asm("NOP");
            asm("NOP");
            
            //Retrigger the Timer
            EVSYS.SWEVENTA = EVSYS_SWEVENTA_CH0_gc;
            timeCycles++;
        }
        
    } while (timeout > timeCycles);
    
    return false;
}
