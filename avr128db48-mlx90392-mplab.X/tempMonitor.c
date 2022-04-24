#include "tempMonitor.h"
#include "MLX90632.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "GPIO.h"
#include "EEPROM_Locations.h"
#include "printUtility.h"
#include "RTC.h"
#include "RN4870.h"
#include "DEFAULTS.h"
#include "LEDcontrol.h"

#include <avr/io.h>
#include <avr/eeprom.h>

//State Machine for the main program
typedef enum  {
    TEMP_SLEEP = 0, TEMP_START, TEMP_WAIT, TEMP_RESULTS, TEMP_ERROR_WAIT, TEMP_ERROR
} TemperatureMeasState;

//Temperature Measurement State Machine
static TemperatureMeasState tempState = TEMP_SLEEP;

//Set if results are ready to be printed
static bool temperatureResultsReady = false, runInSleep = false;

//Temperature Warning Points
static float tempWarningH = DEFAULT_TEMP_WARNING_H, tempWarningL = DEFAULT_TEMP_WARNING_L;

//If set to 'F', results will be printed in Fahrenheit
//Threshold inputs will be considered as numbers in Fahrenheit
static char tempUnit = DEFAULT_TEMP_UNIT;

//Init the Temp Monitor
void tempMonitor_init(bool safeStart)
{
    bool success;
    
    USB_sendStringRaw("Initializing MLX90632 Temperature Sensor...");
    if (safeStart)
    {
        //Button is Held - Safe Mode
        success = MLX90632_initDevice(true);
    }
    else
    {
        //Normal Init Sequence
        success = MLX90632_initDevice(false);
    }
            
    //If Sensor EEPROM was successfully loaded...
    if (MLX90632_cacheOK())
    {
        USB_sendStringRaw("\r\nLoaded cached constants and settings...");
        tempMonitor_loadSettings(true);
    }
    else
    {
        USB_sendStringRaw("\r\nLoaded constants from sensor and reset to defaults...");
        tempMonitor_loadSettings(false);
                
    }
    
    //Print Result
    if (success)
    {
        USB_sendStringWithEndline("OK");
    }
    else
    {
        USB_sendStringWithEndline("FAILED");
        tempState = TEMP_ERROR;
    }
}

//Load Temp. Sensor Settings and Constants
//If nReset = false, defaults will be loaded
void tempMonitor_loadSettings(bool nReset)
{
    if (!nReset)
    {
        //Reset to Defaults
        
        eeprom_write_word((uint16_t*) TEMP_TRIGGER_PERIOD, DEFAULT_RTC_COMPARE);
        RTC_setCompare(DEFAULT_RTC_COMPARE);
        
        eeprom_write_float((float*) TEMP_WARNING_HIGH_LOCATION, DEFAULT_TEMP_WARNING_H);
        tempWarningH = DEFAULT_TEMP_WARNING_H;
        
        eeprom_write_float((float*) TEMP_WARNING_LOW_LOCATION, DEFAULT_TEMP_WARNING_L);
        tempWarningL = DEFAULT_TEMP_WARNING_L;
        
        eeprom_write_byte((char*) TEMP_UNIT_LOCATION, DEFAULT_TEMP_UNIT);
        tempUnit = DEFAULT_TEMP_UNIT;
        
        eeprom_write_byte((bool*) TEMP_MONITOR_SLEEP_ENABLE, false);
        runInSleep = false;
    }
    else
    {
        //Load Settings
        
        //Update RTC Timer
        RTC_setCompare(eeprom_read_word((uint16_t*) TEMP_TRIGGER_PERIOD));
        
        //Load Stored Values
        tempWarningH = eeprom_read_float((float*) TEMP_WARNING_HIGH_LOCATION);
        tempWarningL = eeprom_read_float((float*) TEMP_WARNING_LOW_LOCATION);
        tempUnit = eeprom_read_byte((char*) TEMP_UNIT_LOCATION);
        runInSleep = eeprom_read_byte((bool*) TEMP_MONITOR_SLEEP_ENABLE);
    }
}


//Sets the temperature unit for the demo. C - Celsius (default), F - Fahrenheit, K - Kelvin
bool tempMonitor_setUnit(char unit)
{
    if (!((unit == 'F') || (unit == 'C') || (unit == 'K')))
    {
        RN4870_sendStringToUser("[ERR] Invalid Unit Specifier");
        USB_sendStringWithEndline("[ERR] Invalid Unit Specifier");
        return false;
    }
    
    tempUnit = unit;
    
    //Store New Value
    eeprom_write_byte((char*) TEMP_UNIT_LOCATION, tempUnit);
    return true;
}

//Returns the current temperature unit
void tempMonitor_printUserSettings(void)
{
    //Print User Settings
    RN4870_sendRawStringToUser("Monitor Temperature in Sleep: ");
    
    if (runInSleep)
    {
        RN4870_sendStringToUser("Yes");
    }
    else
    {
        RN4870_sendStringToUser("No");
    }
    
    sprintf(RN4870_getCharBuffer(), "Temperature Unit: %c\r\nTemp Alarm High: %2.1f%c\r\n"
            "Temp Alarm Low: %2.1f%c\r\n", tempUnit, tempMonitor_getTempWarningHigh(), 
            tempUnit, tempMonitor_getTempWarningLow(), tempUnit);
    RN4870_printBufferedString();
}

//Updates the RTC's sample rate and stores it in EEPROM
void tempMonitor_updateSampleRate(uint16_t sampleRate)
{
    //Update RTC Compare
    RTC_setCompare(sampleRate);
    
    //Store new period
    eeprom_write_word((uint16_t*) TEMP_TRIGGER_PERIOD, sampleRate);

    //    sprintf(USB_getCharBuffer(), "New RTC Compare: 0x%x\r\n", sampleRate);
    //    USB_sendBufferedString();
}

//Sets whether the sensor can run in sleep
void tempMonitor_setRunInSleep(bool enable)
{
    runInSleep = enable;
    eeprom_write_byte((bool*) TEMP_MONITOR_SLEEP_ENABLE, enable);
    
    if (runInSleep)
        USB_sendStringWithEndline("Enabled Temp in Sleep");
    else
        USB_sendStringWithEndline("Disabled Temp in Sleep");
}

//Returns true if the sensor can run in sleep
bool tempMonitor_getRunInSleep(void)
{
    return runInSleep;
}

//Returns true if the temp is abnormal
bool tempMonitor_isTempNormal(void)
{
    //If in sleep AND not enabled...
    if ((RN4870_canSleep()) && (!tempMonitor_getRunInSleep()))
    {
        return true;
    }
    
    float objTemp = MLX90632_getObjectTemp();
    
    if (objTemp >= tempWarningH)
    {
        //High Room Temp
        RN4870_sendStringToUser("[WARN] Room Temperature High");
        USB_sendStringWithEndline("[WARN] Room Temperature High");
        return false;
    }
    else if (objTemp <= tempWarningL)
    {
        //Low Room Temp
        RN4870_sendStringToUser("[WARN] Room Temperature Low");
        USB_sendStringWithEndline("[WARN] Room Temperature Low");
        return false;
    }
    
    return true;
}

//Run the Temp Monitor's Finite State Machine
void tempMonitor_FSM(void)
{
    bool success;
    //MLX90632 Temperature Sensor State Machine
    switch (tempState)
    {
        case TEMP_SLEEP:
        {
            break;
        }
        case TEMP_START:
        {            
            //Start a temperature measurement
            success = MLX90632_startSingleConversion();

            //Move to the next state
            if (!success)
            {               
                USB_sendStringWithEndline("[ERR] Failed to start temp conversion in TEMP_START");
                tempState = TEMP_ERROR;
            }
            else
            {
                USB_sendStringWithEndline("Started new temp conversion in TEMP_START");
                tempState = TEMP_WAIT;
            }

            break;
        }
        case TEMP_WAIT:
        {
            if (MLX90632_isDataReady())
            {
                //Get Results
                success = MLX90632_getResults();

                //Move to the next state
                if (!success)
                {
                    USB_sendStringWithEndline("[ERR] Failed to get temp data from MLX90632_getResults()");
                    tempState = TEMP_ERROR;
                }
                else
                {
                    tempState = TEMP_RESULTS;
                }
            }
            break;
        }
        case TEMP_RESULTS:
        {
            //Compute Temperature
            success = MLX90632_computeTemperature();

            //Update State
            if (success)
            {
                temperatureResultsReady = true;
                tempState = TEMP_SLEEP;
            }
            else
            {
                USB_sendStringWithEndline("[ERR] Failed to compute temp from MLX90632_computeTemperature()");
                tempState = TEMP_ERROR;
            }
            
            break;
        }
        case TEMP_ERROR_WAIT:
        {
            //Wait...
            break;
        }
        case TEMP_ERROR:
        default:
        {
            //Sensor Error has Occurred
            RN4870_sendStringToUser("Error - Temperature sensor communication failure. Reboot device");
            tempState = TEMP_ERROR_WAIT;
        }
    }
}

//Call this function, or attach to an ISR, to request a new conversion
void tempMonitor_requestConversion(void)
{
    if (tempState == TEMP_SLEEP)
    {
        tempState = TEMP_START;
    }
    if (tempState == TEMP_ERROR_WAIT)
    {
        tempState = TEMP_ERROR;
    }
}

//Returns true if results are ready
bool tempMonitor_getResultStatus(void)
{
    return temperatureResultsReady;
}

void tempMonitor_printResults(void)
{   
    //If results aren't ready...
    if (!temperatureResultsReady)
    {
        USB_sendStringWithEndline("[ERR] Temperature results not ready for printing.");
        sprintf(USB_getCharBuffer(), "RTC.CMP = 0x%x\r\n, RTC.PER = 0x%x\r\n", RTC_getCompare(), RTC_getPeriod());
        USB_sendBufferedString();

        //Send String
        RN4870_sendStringToUser("Temperature results are not ready.");
        
        return;
    }
    //Clear Flag
    temperatureResultsReady = false;
    
    //Normal Mode, Print to String
    tempMonitor_printLastResults();
}

void tempMonitor_printLastResults(void)
{
    //Get temp (in Celsius)
    float sensorTemp, objTemp;
    sensorTemp = MLX90632_getSensorTemp();
    objTemp = MLX90632_getObjectTemp();
    
    if (objTemp >= tempWarningH)
    {
        //High Room Temp
        RN4870_sendStringToUser("[WARN] Room Temperature High");
        USB_sendStringWithEndline("[WARN] Room Temperature High");
    }
    else if (objTemp <= tempWarningL)
    {
        //Low Room Temp
        RN4870_sendStringToUser("[WARN] Room Temperature Low");
        USB_sendStringWithEndline("[WARN] Room Temperature Low");
    }
    
    if (tempUnit == 'F')
    {
        //Convert to Fahrenheit
        sensorTemp = (sensorTemp * 1.8) + 32;
        objTemp = (objTemp * 1.8) + 32;
    }
    else if (tempUnit == 'K')
    {
        //Convert to Kelvin
        sensorTemp += 273.15;
        objTemp += 273.15;
    }
    else if (tempUnit != 'C')
    {
        //Invalid Units
        
        //Print Constant String
        USB_sendStringRaw("[WARN] Invalid Unit Specifier for Temperature: ");
        
        //Then call sprintf to print the value
        sprintf(USB_getCharBuffer(), "%c\r\n", tempUnit);
        USB_sendBufferedString();
    }
    
    //Queue Data to Send
    sprintf(RN4870_getCharBuffer(), "Sensor Temperature: %2.1f%c\r\nRoom Temperature: %2.1f%c\r\n",
        sensorTemp, tempUnit, objTemp, tempUnit);
    
    //Print String
    RN4870_printBufferedString();
}

//Sets the warning temp for high temperatures. Temp units are auto-converted from current set to C 
void tempMonitor_setTempWarningHigh(float temp)
{   
    //Convert to C
    if (tempUnit == 'F')
    {
        //Fahrenheit
        temp = (temp - 32.0) * 0.55555;
    }
    else if (tempUnit == 'K')
    {
        //Kelvin
        temp -= 273.15;
    }
    else if (tempUnit != 'C')
    {
        //Invalid Units
        
        //Print Constant String
        USB_sendStringRaw("[WARN] Invalid Unit Specifier for Temperature: ");
        
        //Then call sprintf to print the value
        sprintf(USB_getCharBuffer(),"%c\r\n", tempUnit);
        USB_sendBufferedString();
    }
    
    //Update Value
    tempWarningH = temp;
    
    //Write Value
    eeprom_write_float((float*) TEMP_WARNING_HIGH_LOCATION, tempWarningH);
}

//Returns the warning temp for low temperatures. Temp units are auto-converted from C to current
float tempMonitor_getTempWarningHigh(void)
{
    float val = tempWarningH;
    
    if (tempUnit == 'F')
    {
        val = (tempWarningH * 1.8) + 32;
    }
    else if (tempUnit == 'K')
    {
        val += 273.15;
    }
    
    return val;
}

//Sets the warning temp for low temperatures. Temp units are auto-converted from current set to C 
void tempMonitor_setTempWarningLow(float temp)
{
    //Convert to C
    if (tempUnit == 'F')
    {
        //Fahrenheit
        temp = (temp - 32.0) * 0.55555;
    }
    else if (tempUnit == 'K')
    {
        //Kelvin
        temp -= 273.15;
    }
    else if (tempUnit != 'C')
    {
        //Invalid Units
        
        //Print Constant String
        USB_sendStringRaw("[WARN] Invalid Unit Specifier for Temperature: ");
        
        //Then call sprintf to print the value
        sprintf(USB_getCharBuffer(), "%c\r\n", tempUnit);
        USB_sendBufferedString();
    }
    
    //Update Value
    tempWarningL = temp;
    
    //Write Value
    eeprom_write_float((float*) TEMP_WARNING_HIGH_LOCATION, tempWarningL);
}

//Returns the warning temp for low temperatures. Temp units are auto-converted from C to current
float tempMonitor_getTempWarningLow(void)
{
    float val = tempWarningL;
    
    if (tempUnit == 'F')
    {
        val = (tempWarningL * 1.8) + 32;
    }
    else if (tempUnit == 'K')
    {
        val += 273.15;
    }
    
    return val;

}