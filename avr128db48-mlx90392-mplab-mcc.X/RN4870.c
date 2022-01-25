#include "RN4870.h"

#include "GPIO.h"

#include <avr/io.h>
#include <stdint.h>

#include "printUtility.h"
#include "RN4870_RX.h"

//Power States of RN4870
typedef enum {
    RN4870_POWER_OFF = 0, RN4870_POWERING_UP, RN4870_READY
} RN4870_STATUS;

static RN4870_STATUS statusRN4870 = RN4870_POWER_OFF;

void RN4870_init(void)
{    
    RN4870_MODE_SetHigh();
    
    USB_sendString("Initializing RN4870...\r\n");
        
    //Init RX Engine
    RN4870RX_init();
            
    USB_sendString("Entering Command Mode...");
    
    bool status = RN4870_enterCommandMode();
    
    if (!status)
    {
        USB_sendString("FAILED\r\n");
        RN4870_exitCommandMode();
        return;
    }
    
    USB_sendString("OK\r\n");
            
    //Update BLE Name
    RN4870_sendCommandAndPrint("S-,MCHP-MLX", 255);     
    
    //Exit CMD Mode
    RN4870_exitCommandMode();
    
    //Ready to send data
    statusRN4870 = RN4870_READY;
}

//Checks for events from the RN4870
void RN4870_checkForEvents(void)
{
    if (RN4870RX_isResponseComplete())
    {
        //If %REBOOT% has been received...
        if (RN4870RX_compareStatus("REBOOT"))
        {
            statusRN4870 = RN4870_READY;
        }
        
        //Clear Status Flag
        RN4870RX_clearStatusRX();
    }
}

//Powers up the RN4870. Non-Blocking
void RN4870_powerUp(void)
{
    //TODO: Implement Power Up
    
    //Already powered / powering up
    if (statusRN4870 != RN4870_POWER_OFF)
    {
        return;
    }
    
    statusRN4870 = RN4870_READY;
}

//Powers down the RN4870. Non-Blocking
void RN4870_powerDown(void)
{    
    //TODO: Implement Power Down
    statusRN4870 = RN4870_POWER_OFF;
}

bool RN4870_enterCommandMode(void)
{
    //Clear the Watchdog Timer
    asm("WDR");
    
    //Set Mode Switch to Low
    RN4870_MODE_SetLow();
    
    //Enter Command Mode
    BLE_sendString("$$$");
        
    //Wait for RN4870 to enter command mode
    return RN4870RX_waitForCommandRX(100);
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
    return (statusRN4870 == RN4870_READY);
}

//Sends a string to the user, if powered up.
void RN4870_sendStringToUser(const char* str)
{
    if (statusRN4870 != RN4870_READY)
    {
        //Not Ready to Send Data...
        return;
    }
    
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
    if (statusRN4870 != RN4870_READY)
    {
        //Not Ready to Send Data...
        return;
    }
    
    BLE_sendBufferedString();
}
