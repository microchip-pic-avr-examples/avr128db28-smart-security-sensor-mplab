#include "Welcome_Timer.h"

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

static bool run = false;

//Start the Welcome Timer
void welcomeTimer_start(void)
{
    welcomeTimer_reset();
    run = true;
}

//Stop the Welcome Timer
void welcomeTimer_stop(void)
{
    run = false;
}

//Returns true if running, false if not
bool welcomeTimer_isRunning(void)
{
    return run;
}

//Add the current delta from the RTC
void welcomeTimer_addTime(void)
{
    //If rolled over or not enabled, skip time increment
    if ((welcomeTimer_hasTriggered() || (!run)))
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
void welcomeTimer_setCurrentTime(void)
{
    lTime = RTC_getCount();
}

//Clear Trigger Flag and Count
void welcomeTimer_reset(void)
{
    cycleCount = 0;
    lTime = RTC_getCount();
}

//Returns true if the timer has triggered
bool welcomeTimer_hasTriggered(void)
{   
    //If disabled
    if (!run)
        return false;
    
    return (cycleCount >= WELCOME_DELAY_BLUETOOTH);
}