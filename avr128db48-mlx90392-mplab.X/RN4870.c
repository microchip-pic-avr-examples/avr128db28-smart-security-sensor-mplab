#include "RN4870.h"

#include "GPIO.h"

#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "printUtility.h"
#include "RN4870_RX.h"

//Power States of RN4870
typedef enum {
    RN4870_POWER_OFF = 0, RN4870_POWERING_UP_INIT, RN4870_POWERING_UP, RN4870_RECONNECT, 
            RN4870_PAIRING, RN4870_READY, RN4870_POWERING_DOWN
} RN4870_STATUS;

static RN4870_STATUS stateRN4870 = RN4870_POWERING_UP_INIT;
static bool (*onUserEvent)(void) = 0;

void RN4870_init(void)
{            
    //Init RX Engine
    RN4870RX_init();
}

//Setup on initial power-up
bool RN4870_startupInit(void)
{
    USB_sendString("Beginning RN4870 Initial Power-Up Config...\r\n");
    
    bool status = RN4870_enterCommandMode();
    
    if (!status)
    {
        RN4870_exitCommandMode();
        return false;
    }
                
    //Enable UART Transparent Service
    RN4870_sendCommandAndPrint("SS,40", 255);    
    
    //Update BLE Name
    RN4870_sendCommandAndPrint("S-,MCHP-MLX", 255);     
    
    //Reboot!
    RN4870_sendCommandAndPrint("R,1", 0);
    
    return true;
}

//Sets a user event handler
void RN4870_setUserEventHandler(bool (*userEvent)(void))
{
    onUserEvent = userEvent;
}

//Polls for events and runs the power-up/down state machine
void RN4870_processEvents(void)
{
    //Clear Watchdog, as this could be slow
    asm("WDR");
    
#ifdef RN4870_NO_POWER_GATE_TEST
    if (stateRN4870 == RN4870_POWERING_UP_INIT)
    {
        RN4870_startupInit();
        stateRN4870 = RN4870_POWERING_UP;
    }
#endif
    
    while (!RN4870RX_isEmpty())
    {
        //Process the message (both functions check type before running)
        RN4870_runUserCommands();
        RN4870_processStatusMessages();
        
        //Advance to the next message
        RN4870RX_advanceMessage();
    }
}
    
//Returns a status event, if one is available
RN4870_EVENT RN4870_getStatusEvent(void)
{
    if (RN4870RX_isStatusRX())
    {
        //Clear Status Flag
        RN4870RX_clearStatusRX();
        
        sprintf(USB_getCharBuffer(), "Received Status Message: %s\r\n", RN4870RX_getMessageBuffer());
        USB_sendBufferedString();
        
        if (RN4870RX_searchMessage("REBOOT"))
        {
            return RN4870_EVENT_REBOOT;
        }
        else if (RN4870RX_searchMessage("DISCONNECT"))
        {
            return RN4870_EVENT_DISCONNECT;
        }
        else if (RN4870RX_searchMessage("STREAM_OPEN"))
        {
            return RN4870_EVENT_STREAM_OPEN;
        }
    }
    return RN4870_EVENT_NONE;
}


//Checks for events from the RN4870
void RN4870_runUserCommands(void)
{
    //If a user command was received
    if (RN4870RX_isUserRX())
    {        
        sprintf(USB_getCharBuffer(), "Received User Command: %s\r\n", RN4870RX_getMessageBuffer());
        USB_sendBufferedString();

        //If set, call user event handler
        if (onUserEvent)
        {
            if (onUserEvent())
            {
                RN4870_sendStringToUser("<CMD OK>\r\n");
            }
            else
            {
                RN4870_sendStringToUser("<CMD BAD>\r\n");
            }
        }    
    }
    else
    {
        sprintf(USB_getCharBuffer(), "Non-User Command: %s\r\n", RN4870RX_getMessageBuffer());
        USB_sendBufferedString();
    }
}

//Handles status messages from the RN4870 (called by runStateMachine)
void RN4870_processStatusMessages(void)
{
    //Get Status Event
    RN4870_EVENT event = RN4870_getStatusEvent();

    switch (stateRN4870)
    {
        case RN4870_POWER_OFF:
        {
            break;
        }
        case RN4870_POWERING_UP_INIT:
        {
            //if (event == RN4870_EVENT_REBOOT)
            {
                //Power-Up Init!
                if (RN4870_startupInit())
                {
                    //Wait for Reboot...
                    stateRN4870 = RN4870_POWERING_UP;
                }
            }
            break;
        }
        case RN4870_POWERING_UP:
        {
            if (event == RN4870_EVENT_REBOOT)
            {              
                //Init is done, try to reconnect
                RN4870_sendCommandAndPrint("C", 0);

                stateRN4870 = RN4870_RECONNECT;
            }
            else if (event == RN4870_EVENT_CONNECT)
            {
                //Enable UART Service
                RN4870_sendCommandAndPrint("I", 25);
                
                stateRN4870 = RN4870_READY;
            }
            break;
        }
        case RN4870_RECONNECT:
        {
            //Attempt to reconnect

            //If we already connected, leave this state
            if (event == RN4870_EVENT_STREAM_OPEN)
            {
                //Bond to the connected device
                //RN4870_sendCommandAndPrint("B", 100);
                stateRN4870 = RN4870_READY;
            }
            else if (event == RN4870_EVENT_CONNECT)
            {
                //Enable UART Service
                RN4870_sendCommandAndPrint("I", 25);
            }

            break;
        }
        case RN4870_READY:
        {
            if (event == RN4870_EVENT_DISCONNECT)
            {
                //Disconnected...
                
                //stateRN4870 = RN4870_POWERING_DOWN;
            }
            break;
        }
        case RN4870_POWERING_DOWN:
        {
            //TODO: Implement Power Down
            
            stateRN4870 = RN4870_POWER_OFF;
            break;
        }
    }
}

//Powers up the RN4870. Non-Blocking
void RN4870_powerUp(void)
{
    //TODO: Implement Power Up
    
    //Already powered / powering up
    if (stateRN4870 != RN4870_POWER_OFF)
    {
        return;
    }
    
    stateRN4870 = RN4870_READY;
}

//Powers down the RN4870. Non-Blocking
void RN4870_powerDown(void)
{    
    //TODO: Implement Power Down
    stateRN4870 = RN4870_POWER_OFF;
}

bool RN4870_enterCommandMode(void)
{
    //Clear the Watchdog Timer
    asm("WDR");
    
    //Set Mode Switch to Low
    RN4870_MODE_SetLow();
    
    //Enter Command Mode
    BLE_sendString("$$$");
    
    USB_sendString("RN4870 Entering Command Mode...");
    
    //Wait for RN4870 to enter command mode
    bool status = RN4870RX_waitForCommandRX(100);
    
    if (status)
    {
        USB_sendString("OK\r\n");
    }
    else
    {
        USB_sendString("FAILED\r\n");
    }
    
    return status;
}

void RN4870_exitCommandMode(void)
{
    //Exit CMD Mode
    BLE_printCommandString("---", RN4870_DELIM_RESP);
    
    //Clear buffers... anything received is from CMD mode
    RN4870RX_clearBuffer();
    
    //Move to Data Mode
    RN4870_MODE_SetHigh();

}

bool RN4870_sendCommand(const char* string, uint8_t timeout)
{
    //Clear the Watchdog Timer
    asm("WDR");
    
    BLE_sendString(string);
    return RN4870RX_waitForResponseRX(timeout, RN4870_AOK);
}

void RN4870_sendCommandAndPrint(const char* string, uint8_t timeout)
{
    //Clear the Watchdog Timer
    asm("WDR");
    
    //Debug Print
    USB_sendString("Executing Command: \"");
    USB_sendString(string);
    USB_sendString("\"...");
    
    //Print Command to BLE
    BLE_printCommandString(string, RN4870_DELIM_RESP);
    bool status = RN4870RX_waitForResponseRX(timeout, RN4870_AOK);
    
    if (status)
    {
        USB_sendString("OK\r\n");
    }
    else
    {
        USB_sendString("FAILED\r\n");
    }
}

//Returns true if ready to send data
bool RN4870_isReady(void)
{
    return (stateRN4870 == RN4870_READY);
}

//Sends a string to the user, if powered up.
void RN4870_sendStringToUser(const char* str)
{
//    if (stateRN4870 != RN4870_READY)
//    {
//        //Not Ready to Send Data...
//        return;
//    }
    
    BLE_sendString(str);
}

//Returns the char buffer associated with the BLE
char* RN4870_getCharBuffer(void)
{
    return BLE_getCharBuffer();
}

//Sends the cached string to the user, if powered up
void RN4870_printBufferedString(void)
{
//    if (stateRN4870 != RN4870_READY)
//    {
//        //Not Ready to Send Data...
//        return;
//    }
    
    BLE_sendBufferedString();
}
