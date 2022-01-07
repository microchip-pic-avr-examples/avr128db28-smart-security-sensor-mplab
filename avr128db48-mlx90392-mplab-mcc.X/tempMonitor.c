#include "tempMonitor.h"
#include "MLX90632.h"

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#include "mcc_generated_files/system/system.h"
#include "EEPROM_Utility.h"
#include "EEPROM_Locations.h"
#include "mcc_generated_files/timer/delay.h"
#include "printUtility.h"

//State Machine for the main program
typedef enum  {
    TEMP_SLEEP = 0, TEMP_START, TEMP_WAIT, TEMP_RESULTS, TEMP_ERROR
} TemperatureMeasState;

static volatile TemperatureMeasState tempState = TEMP_SLEEP;

//Init the Temp Monitor
void tempMonitor_init(bool safeStart)
{
    bool success;
    
    printConstantStringUSB("Initializing MLX90632 Temperature Sensor...");
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
            
    //EEPROM was successfully loaded
    if (MLX90632_cacheOK())
    {
        //Load EEPROM Value for RTC Period
        printConstantStringUSB("\r\nLoaded cached constants and settings...");
        
        //Update RTC Timer
        RTC_WritePeroid(get16BFromEEPROM(TEMP_UPDATE_PERIOD));
    }
    else
    {
        //Write the default RTC Period to EEPROM
        printConstantStringUSB("\r\nLoaded constants from sensor and reset to defaults...");
        
        //Write default RTC period to EEPROM
        save16BToEEPROM(TEMP_UPDATE_PERIOD, RTC_ReadPeriod());
    }
    
    //Print Result
    if (success)
    {
        printConstantStringUSB("OK\r\n");
    }
    else
    {
        printConstantStringUSB("FAILED\r\n");
        tempState = TEMP_ERROR;
    }
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
                tempState = TEMP_ERROR;
            }
            else
            {
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
            LED0_Toggle();

            //Compute Temperature
            success = MLX90632_computeTemperature();

            //Update State
            if (success)
            {
                sprintf(getCharBufferUSB(), "Sensor Temperature: %2.2fC\r\nRoom Temperature: %2.2fC\r\n",
                        MLX90632_getSensorTemp(), MLX90632_getObjectTemp());
                printBufferedStringUSB();
                tempState = TEMP_SLEEP;
            }
            else
            {
                tempState = TEMP_ERROR;
            }
            
            break;
        }
        case TEMP_ERROR:
        default:
        {
            //Sensor Error has Occurred
            printConstantStringUSB("Temperature Sensor Error - Reboot Device\r\n");
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
}
