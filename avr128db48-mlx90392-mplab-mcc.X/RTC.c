#include "RTC.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>

void (*PIT_handler)(void);
void (*RTC_OVF_handler)(void);

void RTC_init(void)
{
    //Clear Callbacks
    PIT_handler = 0;
    RTC_OVF_handler = 0;
    
    //Enable run in debug for RTC and PIT
    RTC.DBGCTRL = RTC_DBGRUN_bm;
    RTC.PITDBGCTRL = RTC_PITEN_bm;
    
    //Enable Interrupt on Overflow
    RTC.INTCTRL = RTC_OVF_bm;
    
    //Run from 32.768kHz Internal Oscillator
    RTC.CLKSEL = RTC_CLKSEL_OSC32K_gc;
    
    //Set Period to 15s 
    //0x0F00 = 3840
    RTC.PER = 0x0F00;
    
    //Run the RTC in Standby, Prescaler is 128, Enable RTC
    RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV128_gc | RTC_RTCEN_bm;
    
    //Enable PIT Interrupt
    RTC.PITINTCTRL = RTC_PI_bm;
    
    //Enable Clock Pre-scaler 128, Enable PIT
    RTC.PITCTRLA = RTC_PERIOD_CYC128_gc | RTC_PITEN_bm;
}

//Set the OVF Callback Function
void RTC_setOVFCallback(void (*func)(void))
{
    RTC_OVF_handler = func;
}

//Set the PIT Callback Function
void RTC_setPITCallback(void (*func)(void))
{
    PIT_handler = func;
}

uint16_t RTC_getPeriod(void)
{
    return RTC.PER;
}

void RTC_setPeriod(uint16_t period)
{
    //Wait for Sync...
    while (RTC.STATUS & RTC_PERBUSY_bm);
    
    //Write Value
    RTC.PER = period;
}

ISR(RTC_CNT_vect)
{
    if (RTC.INTFLAGS | RTC_OVF_bm)
    {
        //Overflow Flag
        
        //If callback is set...
        if (RTC_OVF_handler)
        {
            RTC_OVF_handler();
        }
    }
    
    //Clear OVF Flag
    RTC.INTFLAGS |= RTC_OVF_bm;
}

ISR(RTC_PIT_vect)
{
    //If callback is set...
    if (PIT_handler)
    {
        PIT_handler();
    }
    
    //Clear PIT Interrupt
    RTC.PITINTFLAGS |= RTC_PI_bm;
}