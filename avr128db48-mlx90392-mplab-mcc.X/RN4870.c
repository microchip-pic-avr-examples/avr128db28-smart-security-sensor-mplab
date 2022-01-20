#include "RN4870.h"

#include "mcc_generated_files/system/system.h"

#include <xc.h>
#include <stdint.h>

#include "printUtility.h"
#include "mcc_generated_files/timer/delay.h"
#include "RN4870_RX.h"

void RN4870_init(void)
{
    RN4870_MODE_SetHigh();
    
    printConstantStringUSB("Initializing RN4870...\r\n");
        
    //Init RX Engine
    RN4870RX_init();
            
    printConstantStringUSB("Entering Command Mode...");
    
    bool status = RN4870_enterCommandMode();
    
    if (!status)
    {
        printConstantStringUSB("FAILED\r\n");
        RN4870_exitCommandMode();
        return;
    }
    
    printConstantStringUSB("OK\r\n");
            
    //Update BLE Name
    RN4870_sendCommandAndPrint("S-,MCHP-MLX", 255);     
    
    //Exit CMD Mode
    RN4870_exitCommandMode();
}

bool RN4870_enterCommandMode(void)
{
    //Clear the Watchdog Timer
    asm("WDR");
    
    //Set Mode Switch to Low
    RN4870_MODE_SetLow();
    
    //Enter Command Mode
    printConstantStringBLE("$$$");
        
    //Wait for RN4870 to enter command mode
    return RN4870RX_waitForCommandRX(100);
}

void RN4870_exitCommandMode(void)
{
    //Exit CMD Mode
    printCommandStringBLE("---", RN4870_DELIM_RESP);
    
    RN4870RX_clearBuffer();
    
    //Move to Data Mode
    RN4870_MODE_SetHigh();

}

bool RN4870_sendCommand(const char* string, uint8_t timeout)
{
    //Clear the Watchdog Timer
    asm("WDR");
    
    printConstantStringBLE(string);
    return RN4870RX_waitForResponseRX(timeout, RN4870_AOK);
}

void RN4870_sendCommandAndPrint(const char* string, uint8_t timeout)
{
    //Clear the Watchdog Timer
    asm("WDR");
    
    //Debug Print
    printConstantStringUSB("Executing Command: \"");
    printConstantStringUSB(string);
    printConstantStringUSB("\"...");
    
    //Print Command to BLE
    printCommandStringBLE(string, RN4870_DELIM_RESP);
    bool status = RN4870RX_waitForResponseRX(timeout, RN4870_AOK);
    
    if (status)
    {
        printConstantStringUSB("OK\r\n");
    }
    else
    {
        printConstantStringUSB("FAILED\r\n");
    }
}