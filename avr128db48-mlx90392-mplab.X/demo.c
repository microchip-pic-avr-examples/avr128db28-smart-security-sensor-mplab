#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "tempMonitor.h"
#include "windowAlarm.h"
#include "RN4870.h"
#include "RN4870_RX.h"
#include "printUtility.h"

#include "demo.h"
#include "RTC.h"

bool DEMO_handleUserCommands(void)
{   
    bool ok = false;
    
    //Parameter for the command
    char param[DEMO_PARAM_LENGTH];
    RN4870RX_copyMessage(&param[0], DEMO_PARAM_LENGTH);
    
    if (RN4870RX_find("STU"))
    {
        //STU = Set Temp Units
        
        USB_sendString("Running STU Command\r\n");
        //Sets the temp units to the user parameter
        
        if (param != NULL)
        {
            //Execute Command
            tempMonitor_setUnit(param[0]);
            
            //Update Success
            ok = true;
        }
        else
        {
            USB_sendString("[ERR] No parameter found in STU command.\r\n");
        }
    }
    else if (RN4870RX_find("STWL"))
    {
        //STWL - Set Temp Warning Low
        
        USB_sendString("Running STWL Command\r\n");
        
        if (param != NULL)
        {
            float result = atof(param);
            
            //Update the Temp Alarm
            tempMonitor_setTempWarningLow(result);
            
            //Update Success
            ok = true;
        }
        else
        {
            USB_sendString("[ERR] No parameter found in STWL command.\r\n");
        }
    }
    else if (RN4870RX_find("STWH"))
    {
        //STWH - Set Temp Warning High
        
        USB_sendString("Running STWH Command\r\n");
        
        if (param != NULL)
        {
            float result = atof(param);
            
            //Update the Temp Alarm
            tempMonitor_setTempWarningHigh(result);
            
            //Update Success
            ok = true;
        }
        else
        {
            USB_sendString("[ERR] No parameter found in STWH command.\r\n");
        }
    }
    else if (RN4870RX_find("STSR"))
    {
        //STSR - Set Temperature Sampling Rate
        USB_sendString("Running STSR Command\r\n");
        
        if (param != NULL)
        {
            if (strcmp("FAST", param) == 0)
            {
                tempMonitor_updateSampleRate(DEMO_SAMPLE_RATE_FAST);
                
                //Update Success
                ok = true;
            }
            else if (strcmp("NORM", param) == 0)
            {
                tempMonitor_updateSampleRate(DEMO_SAMPLE_RATE_NORM);
                
                //Update Success
                ok = true;
            }
            else if (strcmp("SLOW", param) == 0)
            {
                tempMonitor_updateSampleRate(DEMO_SAMPLE_RATE_SLOW);
                
                //Update Success
                ok = true;
            }
            else
            {
                USB_sendString("[ERR] Invalid parameter found in STSR command.\r\n");
                return false;
            }
        }
        else
        {
            USB_sendString("[ERR] No parameter found in STSR command.\r\n");
        }
    }
    else if (RN4870RX_find("INFO") || RN4870RX_find("DEMO"))
    {
        //Demo / Info
        RN4870_sendStringToUser("Smart Window Security Sensor and Room Temperature Monitor\r\n"
                "Developed by Microchip Technology Inc. with assistance from Melexis N.V.\r\n"
                "Source Code is available at: github.com/microchip-pic-avr-examples/avr128db48-mlx90392-mplab\r\n");
        ok = true;
    }
    
    return ok;
}

