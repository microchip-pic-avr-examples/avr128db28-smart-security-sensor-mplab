#include "RN4870.h"

#include "GPIO.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "printUtility.h"
#include "RN4870_RX.h"
#include "usart2.h"
#include "usart0.h"
#include "TCA0.h"
#include "windowAlarm.h"
#include "LEDcontrol.h"
#include "RTC.h"
#include "demo.h"
#include "Bluetooth_Timeout_Timer.h"
#include "Welcome_Timer.h"
#include "usart2.h"
#include "MLX90632.h"

#include <avr/interrupt.h>

//Power States of RN4870
typedef enum {
    RN4870_POWER_OFF = 0, RN4870_POWERING_UP_INIT, RN4870_POWERING_UP, RN4870_PAIR,
            RN4870_READY
} RN4870_STATUS;

static RN4870_STATUS stateRN4870 = RN4870_POWER_OFF;
static bool (*onUserEvent)(void) = 0;

ISR(PORTA_PORT_vect)
{
    //Disable IOC
    WAKE_DisableIOC();

    //Restart RN4870
    RN4870_powerUp();

    //Mask the pushbutton signal
    windowAlarm_maskButton();

    //Clear Flag
    WAKE_ClearFlag();
}

void RN4870_init(void)
{
    //Init RX Engine
    RN4870RX_init();

    //Power-up RN4870
    RN4870_powerUp();
}

//Setup on initial power-up
bool RN4870_startupInit(void)
{
    USB_sendStringWithEndline("Beginning RN4870 Power-Up Config...");
    asm("WDR");

    bool status = RN4870_enterCommandMode();

    if (!status)
    {
        RN4870_exitCommandMode();
        return false;
    }

//    if (RN4870_isConnected(255))
//    {
//        //Already Connected
//        stateRN4870 = RN4870_READY;
//        return true;
//    }
//    else
//    {
//
//    }

    //Enable UART Transparent Service
    RN4870_sendCommandAndPrint("SS,40", 255);

    //Update BLE Name
    RN4870_sendCommandAndPrint("S-,MCHP-MLX", 255);

    //Reboot!
    RN4870_reboot();

    return true;
}

//Sets a user event handler
void RN4870_setUserEventHandler(bool (*userEvent)(void))
{
    onUserEvent = userEvent;
}

//Returns true if the module is not active, and the MCU can enter sleep
bool RN4870_canSleep(void)
{
    return (stateRN4870 == RN4870_POWER_OFF);
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
        //Reset Timeout Timer
        BLE_SW_Timer_reset();

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
        USB_sendStringRaw("Received Status Message: ");
        RN4870RX_copyMessage(USB_getCharBuffer(), USB_getCharBufferSize());
        USB_sendBufferedString();
        USB_sendStringRaw("\r\n");

        if (RN4870RX_find("REBOOT"))
        {
            USB_sendStringWithEndline("Processed as REBOOT");
            return RN4870_EVENT_REBOOT;
        }
        else if (RN4870RX_find("DISCONNECT"))
        {
            USB_sendStringWithEndline("Processed as DISCONNECT");
            return RN4870_EVENT_DISCONNECT;
        }
        else if (RN4870RX_find("CONNECT"))
        {
            USB_sendStringWithEndline("Processed as CONNECT");
            return RN4870_EVENT_CONNECT;
        }
        else if (RN4870RX_find("STREAM_OPEN"))
        {
            USB_sendStringWithEndline("Processed as STREAM_OPEN");
            return RN4870_EVENT_STREAM_OPEN;
        }
        else if (RN4870RX_find("CONN_PARAM"))
        {
            USB_sendStringWithEndline("Processed as CONN_PARAM");
            return RN4870_EVENT_CONN_PARAM;
        }
        else
        {
            USB_sendStringWithEndline("[ERR] Unable to match string to response.");
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
        USB_sendStringRaw("Received User Command: ");
        RN4870RX_copyMessage(USB_getCharBuffer(), USB_getCharBufferSize());
        USB_sendBufferedString();
        USB_sendStringRaw("\r\n");

        //If set, call user event handler
        if (onUserEvent)
        {
            if (onUserEvent())
            {
                RN4870_sendStringToUser("<CMD OK>");
            }
            else
            {
                RN4870_sendStringToUser("<CMD BAD>");
            }
        }
    }
    else
    {
        USB_sendStringRaw("Non-User Command: ");
        RN4870RX_copyMessage(USB_getCharBuffer(), USB_getCharBufferSize());
        USB_sendBufferedString();
        USB_sendStringRaw("\r\n");

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
            if (event == RN4870_EVENT_REBOOT)
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
                stateRN4870 = RN4870_PAIR;
            }
            else if (event == RN4870_EVENT_STREAM_OPEN)
            {
                LED_turnOnBlue();
                stateRN4870 = RN4870_READY;

                welcomeTimer_start();
            }
            break;
        }
        case RN4870_PAIR:
        {
            //Attempt to reconnect

            //If we already connected, leave this state
            if (event == RN4870_EVENT_STREAM_OPEN)
            {
                LED_turnOnBlue();
                stateRN4870 = RN4870_READY;

                welcomeTimer_start();
            }

            break;
        }
        case RN4870_READY:
        {
            if (event == RN4870_EVENT_DISCONNECT)
            {
                //Disconnected...
                LED_turnOffBlue();
                stateRN4870 = RN4870_PAIR;
            }
            break;
        }
    }
}

//Powers up the RN4870. Non-Blocking
void RN4870_powerUp(void)
{    
    //Already powered / powering up
    if (stateRN4870 != RN4870_POWER_OFF)
    {
        return;
    }
    
    //Disable IOC
    WAKE_DisableIOC();

    RTC_setPIT(RTC_PERIOD_CYC128_gc);

    //Switch LEDs to PWM Control
    LED_switchToActive();

    //Power-Up the module
    BTLE_EnablePower();

    //Release from nRESET (0)
    BTLE_ReleaseReset();

    //Reinit USART
    USART0_initIO();
    USART0_enableTX();
    USART0_enableRX();

    //Update State
    stateRN4870 = RN4870_POWERING_UP_INIT;
}

//Powers down the RN4870. Non-Blocking
void RN4870_powerDown(void)
{
    //Update State
    stateRN4870 = RN4870_POWER_OFF;

    //Reduce PIT Sampling Rate
    RTC_setPIT(RTC_PERIOD_CYC1024_gc);

    //Hold in nRESET
    BTLE_AssertReset();

    //Disable USART
    USART0_disableTX();
    USART0_disableRX();
    
    //Disable Debug UART
    USART2_disableTX();

    //Power-Down the module
    BTLE_DisablePower();

    //Enable WAKE Pin to resume communication
    WAKE_EnableIOC();

    //Disable LED PWM Driver
    TCA0_disable();

    //Turn off the LEDs
    LED_turnOffRed();
    LED_turnOffGreen();
    LED_turnOffBlue();

    //Clear RTC flags
    RTC_clearCMPTrigger();
    RTC_clearOVFTrigger();

    //Reset Timeout Timer
    BLE_SW_Timer_reset();
}

bool RN4870_enterCommandMode(void)
{
    //Clear the Watchdog Timer
    asm("WDR");

    //Enter Command Mode
    BLE_sendStringRaw("$$$");

    USB_sendStringRaw("RN4870 Entering Command Mode...");

    //Wait for RN4870 to enter command mode
    bool status = RN4870RX_waitForCommandRX(100);

    if (status)
    {
        USB_sendStringRaw("OK\r\n");
    }
    else
    {
        USB_sendStringRaw("FAILED\r\n");
    }

    return status;
}

void RN4870_exitCommandMode(void)
{
    //Exit CMD Mode
    BLE_printCommandString("---", '\r');

    //Clear buffers... anything received is from CMD mode
    RN4870RX_clearBuffer();
}

//Returns true if connected
bool RN4870_isConnected(void)
{
    return (stateRN4870 == RN4870_READY);
}

//Reboots the RN4870
bool RN4870_reboot(void)
{
    //Clear the Watchdog Timer
    asm("WDR");

    //Debug Print
    USB_sendStringRaw("Executing Command: \"");
    USB_sendStringRaw("R,1");
    USB_sendStringRaw("\"...");

    //Print Command to BLE
    BLE_printCommandString("R,1", '\r');
    bool status = true;

    status = RN4870RX_waitForResponseRX(100, "Rebooting");

    if (status)
    {
        USB_sendStringWithEndline("OK");
    }
    else
    {
        USB_sendStringWithEndline("FAILED");
    }

    return true;
}

bool RN4870_sendCommand(const char* string, uint8_t timeout)
{
    //Clear the Watchdog Timer
    asm("WDR");

    BLE_sendStringRaw(string);
    return RN4870RX_waitForResponseRX(timeout, "AOK");
}

void RN4870_sendCommandAndPrint(const char* string, uint8_t timeout)
{
    //Clear the Watchdog Timer
    asm("WDR");

    //Debug Print
    USB_sendStringRaw("Executing Command: \"");
    USB_sendStringRaw(string);
    USB_sendStringRaw("\"...");

    //Print Command to BLE
    BLE_printCommandString(string, '\r');
    bool status = true;

    if (timeout != 0)
        status = RN4870RX_waitForResponseRX(timeout, "AOK");

    if (status)
    {
        USB_sendStringWithEndline("OK");
    }
    else
    {
        USB_sendStringWithEndline("FAILED");
    }
}

//Sends a string to the user, if powered up.
void RN4870_sendStringToUser(const char* str)
{
    if (stateRN4870 != RN4870_READY)
    {
        //Not Ready to Send Data...
        return;
    }

    if ((str == NULL) || (str[0] == '\0'))
    {
        return;
    }

    BLE_sendStringWithEndline(str);
}

//Prints the literal string to the user, if powered up
void RN4870_sendRawStringToUser(const char* str)
{
    if (stateRN4870 != RN4870_READY)
    {
        //Not Ready to Send Data...
        return;
    }

    if ((str == NULL) || (str[0] == '\0'))
    {
        return;
    }

    BLE_sendStringRaw(str);
}

//Returns the char buffer associated with the BLE
char* RN4870_getCharBuffer(void)
{
    return BLE_getCharBuffer();
}

//Sends the cached string to the user, if powered up
void RN4870_printBufferedString(void)
{
    if (stateRN4870 != RN4870_READY)
    {
        //Not Ready to Send Data...
        return;
    }

    BLE_sendBufferedString();
}
