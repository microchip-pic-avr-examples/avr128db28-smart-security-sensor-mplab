#include "Bluetooth_Timeout_Timer.h"

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdio.h>

#include "RTC.h"
#include "DEFAULTS.h"
#include "EEPROM_Locations.h"
#include "RN4870.h"

//Last-time RTC was checked
static uint16_t lTime = 0;

//Elapsed Count
static uint16_t cycleCount = 0;

//Timeout Period
static uint16_t timeoutPeriod = 0;

//Load the Timeout Settings
//If nReset = false, will load default settings
void BLE_SW_Timer_loadSettings(bool nReset)
{
    if (!nReset)
    {
        //Reset Settings
        eeprom_write_word((uint16_t*) SYSTEM_BLUETOOTH_IDLE_PERIOD, DEFAULT_BLUETOOTH_IDLE_PERIOD);
        BLE_SW_Timer_setPeriod(DEFAULT_BLUETOOTH_IDLE_PERIOD);
    }
    else
    {
        //Load Settings
        BLE_SW_Timer_setPeriod(eeprom_read_word((uint16_t*) SYSTEM_BLUETOOTH_IDLE_PERIOD));
    }
}

//Sets and stores the current period to EEPROM
void BLE_SW_Timer_saveSettings(uint16_t period)
{
    eeprom_write_word((uint16_t*) SYSTEM_BLUETOOTH_IDLE_PERIOD, period);
    BLE_SW_Timer_setPeriod(period);
}

void BLE_SW_Timer_printUserSettings(void)
{
    RN4870_sendRawStringToUser("Bluetooth Idle Off: ");
    if (timeoutPeriod == 0)
    {
        RN4870_sendStringToUser("No");
    }
    else
    {
        RN4870_sendStringToUser("Yes");
    }
}

//Set the number of cycles (from the RTC)
void BLE_SW_Timer_setPeriod(uint16_t period)
{
    timeoutPeriod = period;
}

//Add the current delta from the RTC
void BLE_SW_Timer_addTime(void)
{
    //If rolled over, skip time increment
    if (BLE_SW_Timer_hasTriggered())
        return;
    
    if (timeoutPeriod == 0)
        return;
    
    volatile uint16_t cTime = RTC_getCount();
    
    if (cTime >= lTime)
    {
        //Has not rolled over
        cycleCount += (cTime - lTime);
    }
    else
    {
        //Has rolled over
        cycleCount += (RTC_getPeriod() - lTime) + cTime;
    }
    
    lTime = cTime;
}

//Sets the starting count of the RTC
void BLE_SW_Timer_setCurrentTime(void)
{
    lTime = RTC_getCount();
}

//Resets the BLE_SW_Timer
//Clear Trigger Flag and Count
void BLE_SW_Timer_reset(void)
{
    cycleCount = 0;
    lTime = RTC_getCount();
}

//Returns true if the timer has triggered
bool BLE_SW_Timer_hasTriggered(void)
{
    if (timeoutPeriod == 0)
        return false;
    
    return (cycleCount >= timeoutPeriod);
}