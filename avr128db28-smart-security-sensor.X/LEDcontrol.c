#include "LEDcontrol.h"
#include "TCA0.h"
#include "GPIO.h"
#include "RN4870.h"

#include <avr/io.h>

//Turns on/off the Red channel
void LED_turnOnRed(void)
{
    if (RN4870_canSleep())
    {
        IO_LED0R_TurnOn();
    }
    else
    {
        TCA0_enableLEDR();
    }
}
void LED_turnOffRed(void)
{
    IO_LED0R_TurnOff();
    TCA0_disableLEDR();
}

//Turns on/off the Green channel
void LED_turnOnGreen(void)
{
    if (RN4870_canSleep())
    {
        IO_LED0G_TurnOn();
    }
    else
    {
        TCA0_enableLEDG();
    }
}
void LED_turnOffGreen(void)
{
    IO_LED0G_TurnOff();
    TCA0_disableLEDG();
}

//Turns on/off the Blue channel
void LED_turnOnBlue(void)
{
    if (RN4870_canSleep())
    {
        IO_LED0B_TurnOn();
    }
    else
    {
        TCA0_enableLEDB();
    }

}

void LED_turnOffBlue(void)
{
    IO_LED0B_TurnOff();
    TCA0_disableLEDB();
}

//Switches from PWM to IO Control
void LED_switchToSleep(void)
{
    //Red LED
    if (TCA0_getLEDR())
    {
        IO_LED0R_TurnOn();
        TCA0_disableLEDR();
    }
    else
    {
        IO_LED0R_TurnOff();
    }
    
    //Green LED
    if (TCA0_getLEDG())
    {
        IO_LED0G_TurnOn();
        TCA0_disableLEDG();
    }
    else
    {
        IO_LED0G_TurnOff();
    }
    
    //Blue LED
    if (TCA0_getLEDB())
    {
        IO_LED0B_TurnOn();
        TCA0_disableLEDB();
    }
    else
    {
        IO_LED0B_TurnOff();
    }
}

//Switches from IO to PWM Control
void LED_switchToActive(void)
{
    //Red LED
    if (IO_LED0R_GetValue())
    {
        TCA0_enableLEDR();
        IO_LED0R_TurnOff();
    }
    
    //Green LED
    if (IO_LED0G_GetValue())
    {
        TCA0_enableLEDG();
        IO_LED0G_TurnOff();
    }
    
    //Blue LED
    if (IO_LED0B_GetValue())
    {
        TCA0_enableLEDB();
        IO_LED0B_TurnOff();
    }
}