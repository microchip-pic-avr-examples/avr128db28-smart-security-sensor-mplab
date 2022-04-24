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
#include "ADC.h"
#include "Bluetooth_Timeout_Timer.h"
#include "LEDcontrol.h"

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
    BLE_SW_Timer_loadSettings(nReset);
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
        
        USB_sendStringWithEndline("Running STU Command");
        //Sets the temp units to the user parameter
        
        if (paramOK)
        {
            //Execute Command
            ok = tempMonitor_setUnit(param[0]);
        }
        else
        {
            USB_sendStringWithEndline("[ERR] No parameter found in STU command.");
        }
    }
    else if (RN4870RX_find("STWL"))
    {
        //STWL - Set Temp Warning Low
        
        //Copy Parameter
        paramOK = RN4870RX_copyMessageParameter(&param[0], DEMO_PARAM_LENGTH);
        
        USB_sendStringWithEndline("Running STWL Command");
        
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
            USB_sendStringWithEndline("[ERR] No parameter found in STWL command.");
        }
    }
    else if (RN4870RX_find("STWH"))
    {
        //STWH - Set Temp Warning High
        
        //Copy Parameter
        paramOK = RN4870RX_copyMessageParameter(&param[0], DEMO_PARAM_LENGTH);
        
        USB_sendStringWithEndline("Running STWH Command");
        
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
            USB_sendStringWithEndline("[ERR] No parameter found in STWH command.");
        }
    }
    else if (RN4870RX_find("STSR"))
    {
        //STSR - Set Temperature Sampling Rate
        USB_sendStringWithEndline("Running STSR Command");
        
        if (RN4870RX_find(","))
        {
            if (RN4870RX_find("FAST"))
            {
                DEMO_setSystemUpdateRateEEPROM(DEMO_SAMPLE_RATE_FAST);
                
                //Update Success
                ok = true;
            }
            else if (RN4870RX_find("NORM"))
            {
                DEMO_setSystemUpdateRateEEPROM(DEMO_SAMPLE_RATE_NORM);
                
                //Update Success
                ok = true;
            }
            else if (RN4870RX_find("SLOW"))
            {
                DEMO_setSystemUpdateRateEEPROM(DEMO_SAMPLE_RATE_SLOW);
                
                //Update Success
                ok = true;
            }
            else
            {
                USB_sendStringWithEndline("[ERR] Invalid parameter found in STSR command.");
                return false;
            }
        }
        else
        {
            USB_sendStringWithEndline("[ERR] No parameter found in STSR command.");
        }
    }
    else if (RN4870RX_find("INFO") || RN4870RX_find("DEMO"))
    {
        //Demo / Info
        RN4870_sendStringToUser("Smart Window Security Sensor and Room Temperature Monitor\r\n"
                "Developed by Microchip Technology Inc. with assistance from Melexis\r\n"
                "Source Code is available at: github.com/microchip-pic-avr-examples/avr128db48-mlx90392-mplab");
        ok = true;
    }
    else if (RN4870RX_find("VBAT"))
    {
        sprintf(RN4870_getCharBuffer(), "Current Battery Voltage: %1.3fV\r\n", ADC_getResultAsFloat(ADC_MUXPOS_AIN6_gc));
        RN4870_printBufferedString();
        
        ok = true;
    }
    else if (RN4870RX_find("STATUS"))
    {
        //Print Current Results
        RN4870_sendStringToUser("Current System State:");
        
        windowAlarm_printResults();
        tempMonitor_printLastResults();
        
        ok = true;
    }
    else if (RN4870RX_find("USER"))
    {
        //Print User Settings
        
        //Auto Off
        //Temp Unit
        //Temp High, Temp Low
        //Temp in Sleep
        //Calibration State (Good / Bad)
        
        RN4870_sendStringToUser("--Start User Settings--");
        
        //Print Auto Off Status
        BLE_SW_Timer_printUserSettings();
        
        //Print Reporting Rate
        RN4870_sendRawStringToUser("Reporting Rate: ");
        
        uint16_t rtcPeriod = RTC_getPeriod();
        
        if (rtcPeriod == DEMO_SAMPLE_RATE_FAST)
        {
            RN4870_sendStringToUser("Fast (3s)");
        }
        else if (rtcPeriod == DEMO_SAMPLE_RATE_NORM)
        {
            RN4870_sendStringToUser("Normal (15s)");
        }
        else if (rtcPeriod == DEMO_SAMPLE_RATE_SLOW)
        {
            RN4870_sendStringToUser("Slow (30s)");
        }
        else
        {
            sprintf(RN4870_getCharBuffer(), "Other (0x%x)\r\n", rtcPeriod);
            RN4870_printBufferedString();
        }
        
        //Print Magnetometer Calibration
        RN4870_sendRawStringToUser("Magnetometer Calibrated: ");
        
        if (windowAlarm_isCalGood())
        {
            RN4870_sendStringToUser("Yes");
        }
        else
        {
            RN4870_sendStringToUser("No");
        }

        //Print Temp Monitor Settings
        tempMonitor_printUserSettings();
                
        RN4870_sendStringToUser("--End User Settings--");
        
        ok = true;
    }
    else if (RN4870RX_find("RECAL"))
    {
        //Set Window Alarm in Calibration Mode
        windowAlarm_requestCalibration();
        
        ok = true;
    }
    else if (RN4870RX_find("GETCAL"))
    {
        //Prints Calibration Parameters
        
        RN4870_sendStringToUser("--Start Calibration Parameters--");
        windowAlarm_printCalibration();
        RN4870_sendStringToUser("--End Calibration Parameters--");
        
        ok = true;
    }
    else if (RN4870RX_find("MAGRAW"))
    {
        //Request Raw Data from Magnetometer
        
        windowAlarm_requestRawPrint();
        
        RN4870_sendStringToUser("Next measurement will be printed.");
        ok = true;
    }
    else if (RN4870RX_find("RESET"))
    {
        //Reset User Settings
        windowAlarm_loadSettings(false);
        tempMonitor_loadSettings(false);
        DEMO_loadSettings(false);
        
        RN4870_sendStringToUser("Settings were reset.");
        
        ok = true;
    }
    else if (RN4870RX_find("REBOOT"))
    {
        RN4870_powerDown();
        while (1)
        { 
            //Will force the WDT to trigger a restart
        }
    }
    else if ((RN4870RX_find("PWDWN")) || (RN4870RX_find("PWRDWN")))
    {
        //Power down
        
        //If calibration is OK and alarm is inactive
        if ((windowAlarm_isCalGood()) && (windowAlarm_isAlarmOK()))
        {
            RN4870_powerDown();
            ok = true;
        }
        else
        {
            if (!windowAlarm_isCalGood())
            {
                RN4870_sendStringToUser("Please calibrate the window alarm before power down.");
            }
            else
            {
                RN4870_sendStringToUser("Node cannot be powered down if alarm is active.");
            }
        }
    }
    else if (RN4870RX_find("IDLEOFF"))
    {
        //Bluetooth Idle Off
        
        if (RN4870RX_find(","))
        {
            //Parameter found
            if (RN4870RX_find("TRUE"))
            {
                BLE_SW_Timer_saveSettings(DEFAULT_BLUETOOTH_IDLE_PERIOD);
                ok = true;
            }
            else if (RN4870RX_find("FALSE"))
            {
                //Turn off period
                BLE_SW_Timer_saveSettings(0);
                ok = true;
            }
            else
            {
                USB_sendStringWithEndline("[ERR] Improper parameter found in IDLEOFF command.");
            }
        }
        else
        {
            USB_sendStringWithEndline("[ERR] No parameter found in IDLEOFF command.");
        }
    }
    else if (RN4870RX_find("TEMPSLEEP"))
    {
        //Temperature monitor in sleep
        
        //If set to true, the sensor monitors temperatures in sleep
        //If set to false, the sensor is not monitoring for abnormal temps in sleep
        
        if (RN4870RX_find(","))
        {
            //Parameter found
            if (RN4870RX_find("TRUE"))
            {
                tempMonitor_setRunInSleep(true);
                ok = true;
            }
            else if (RN4870RX_find("FALSE"))
            {
                //Turn off period
                tempMonitor_setRunInSleep(false);
                ok = true;
            }
            else
            {
                USB_sendStringWithEndline("[ERR] Improper parameter found in TEMPSLEEP command.");
            }
        }
        else
        {
            USB_sendStringWithEndline("[ERR] No parameter found in BTIDLEOFF command.");
        }

    }
    else
    {
        RN4870_sendStringToUser("Command not found. For help, please type HELP.");
    }
    
    return ok;
}

//Sets the update rate of the demo
void DEMO_setSystemUpdateRateEEPROM(uint16_t rate)
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

//Sets the update rate of the demo
//Does NOT write to EEPROM
void DEMO_setSystemUpdateRateRAM(uint16_t rate)
{    
    RTC_setPeriod(rate);
    
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

//Sets the Red/Green LEDs according to the current state
void DEMO_setAlarmLEDs(void)
{
    if (!windowAlarm_isCalGood())
    {
        //Cal Mode
        BLE_SW_Timer_reset();
        LED_turnOnGreen();
        LED_turnOnRed();
    }
    else if (windowAlarm_isAlarmOK() && tempMonitor_isTempNormal())
    {
        //No Alarm
        LED_turnOffRed();
        LED_turnOnGreen();
    }
    else
    {        
        
        //Alarm
        BLE_SW_Timer_reset();
        LED_turnOnRed();
        LED_turnOffGreen();
    }
}