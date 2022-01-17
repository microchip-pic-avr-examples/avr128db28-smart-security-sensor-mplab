#include "RN4870.h"

#include "mcc_generated_files/system/system.h"

#include <xc.h>
#include <stdint.h>

#include "printUtility.h"
#include "mcc_generated_files/timer/delay.h"
#include "RN4870_RX.h"

void RN4870_init(void)
{
    printConstantStringUSB("Initializing RN4870...");
    
    //Set Mode Switch to Low
    RN4870_MODE_SetLow();
    
    //Init RX Engine
    RN4870RX_init();
        
    DELAY_milliseconds(100);
    
    //Enter Command Mode
    printConstantStringBLE("$$$");
        
    //Wait for RN4870 to enter command mode
    bool status = RN4870RX_waitForCommandRX(100);
    
    if (!status)
    {
        printConstantStringUSB("FAILED\r\n");
    }
    else
    {
        printConstantStringUSB("OK\r\n");
    }
    
    //Update BLE Name
    //printConstantStringBLE("S-,MLX-MCHP\r");
        
    //printConstantStringBLE("A\r");
    
    //Exit CMD Mode
    printConstantStringBLE("---\r");
    
    RN4870RX_clearBuffer();
    
    //Move to Data Mode
    RN4870_MODE_SetHigh();
}
