#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <avr/eeprom.h>

#include "tempMonitor.h"
#include "windowAlarm.h"
#include "RN4870.h"
#include "RN4870_RX.h"
#include "printUtility.h"

#include "demo.h"
#include "RTC.h"
#include "EEPROM_Locations.h"
#include "DEFAULTS.h"

#define DEMO_GOOD_VALUE 0x7A

//Load settings in EEPROM or reset to default (if in safe mode)
void DEMO_init(bool safeStart)
{
    uint8_t value;

    value = eeprom_read_byte((uint8_t*) SYSTEM_GOOD_MARKER);

    if ((!safeStart) && (value == DEMO_GOOD_VALUE))
    {
        //Load Settings
        DEMO_loadSettings(true);
    }
    else
    {
        //If EEPROM validation failed or in safe mode...
        DEMO_loadSettings(false);
    }
}

//Loads or saves default settings.
//If reset = true, will load defaults
void DEMO_loadSettings(bool nReset)
{
    if (!nReset)
    {
        //Write Default Value
        eeprom_write_byte((uint8_t*) SYSTEM_GOOD_MARKER, DEMO_GOOD_VALUE);
        eeprom_write_word((uint16_t*) SYSTEM_UPDATE_PERIOD, DEFAULT_RTC_PERIOD);
        RTC_setPeriod(DEFAULT_RTC_PERIOD);
    }
    else
    {
        //Read Settings
        RTC_setPeriod(eeprom_read_word((uint16_t*) SYSTEM_UPDATE_PERIOD));
    }
}

bool DEMO_handleUserCommands(void)
{   
    bool ok = false, paramOK = false;
    
    char param[DEMO_PARAM_LENGTH];
        
    if (RN4870RX_find("STU"))
    {
        //STU = Set Temp Units
        
        //Copy Parameter
        paramOK = RN4870RX_copyMessageParameter(&param[0], DEMO_PARAM_LENGTH);
        
        USB_sendString("Running STU Command\r\n");
        //Sets the temp units to the user parameter
        
        if (paramOK)
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
        
        //Copy Parameter
        paramOK = RN4870RX_copyMessageParameter(&param[0], DEMO_PARAM_LENGTH);
        
        USB_sendString("Running STWL Command\r\n");
        
        if (paramOK)
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
        
        //Copy Parameter
        paramOK = RN4870RX_copyMessageParameter(&param[0], DEMO_PARAM_LENGTH);
        
        USB_sendString("Running STWH Command\r\n");
        
        if (paramOK)
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
            if (RN4870RX_find("FAST"))
            {
                DEMO_setSystemUpdateRate(DEMO_SAMPLE_RATE_FAST);
                
                //Update Success
                ok = true;
            }
            else if (RN4870RX_find("NORM"))
            {
                DEMO_setSystemUpdateRate(DEMO_SAMPLE_RATE_NORM);
                
                //Update Success
                ok = true;
            }
            else if (RN4870RX_find("SLOW"))
            {
                DEMO_setSystemUpdateRate(DEMO_SAMPLE_RATE_SLOW);
                
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
    else if (RN4870RX_find("STATUS"))
    {
        //Print Current Results
        RN4870_sendStringToUser("Current System State: \r\n");
        
        windowAlarm_printResults();
        tempMonitor_printLastResults();
        
        ok = true;
    }
    else if (RN4870RX_find("RECAL"))
    {
        //Set Window Alarm in Calibration Mode
        windowAlarm_requestCalibration();
        
        ok = true;
    }
    else if (RN4870RX_find("RESET"))
    {
        //Reset User Settings
        windowAlarm_loadSettings(false);
        tempMonitor_loadSettings(false);
        DEMO_loadSettings(false);
        
        RN4870_sendStringToUser("Settings were reset.\r\n");
        
        ok = true;
    }
    else if (RN4870RX_find("PWDWN"))
    {
        //Power down
        RN4870_powerDown();
        
        ok = true;
    }
    
    return ok;
}

//Sets the update rate of the demo
void DEMO_setSystemUpdateRate(uint16_t rate)
{    
    RTC_setPeriod(rate);
    eeprom_write_word((uint16_t*) SYSTEM_UPDATE_PERIOD, rate);
    
    //Calculate Temp Trigger Timing
    if (rate <= DEMO_TEMP_DELAY_START)
    {
        //When RTC.CNT = 0
        rate = 0;
    }
    else
    {
        rate -= DEMO_TEMP_DELAY_START;
    }
    
    //Update Temp Trigger Timing
    tempMonitor_updateSampleRate(rate);
}