#include "RTC.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>
#include <stdbool.h>

void (*PIT_handler)(void) = 0;
void (*RTC_OVF_handler)(void) = 0;
void (*RTC_CMP_handler)(void) = 0;

static volatile bool RTC_PIT_triggered = false;
static volatile bool RTC_OVF_triggered = false;
static volatile bool RTC_CMP_triggered = false;


void RTC_init(void)
{    
    //Enable run in debug for RTC and PIT
    RTC.DBGCTRL = RTC_DBGRUN_bm;
    RTC.PITDBGCTRL = RTC_PITEN_bm;
    
    //Enable Interrupt on Overflow, Compare
    RTC.INTCTRL = RTC_OVF_bm | RTC_CMP_bm;
    
    //Run from 32.768kHz Internal Oscillator
    RTC.CLKSEL = RTC_CLKSEL_OSC32K_gc;
    
    while (RTC.STATUS & RTC_PERBUSY_bm);
    
    //Set Period to 15s 
    //0x0F00 = 3840
    RTC.PER = 0x0F00;
    
    while (RTC.STATUS & RTC_CMPBUSY_bm);
    
    //Set Compare at 13.75s
    //0x0DC0 = 3520
    RTC.CMP = 0x0DC0;
    
    while (RTC.STATUS & RTC_CTRLABUSY_bm);
    
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

//Set the CMP Callback Function
void RTC_setCMPCallback(void (*func)(void))
{
    RTC_CMP_handler = func;
}


uint16_t RTC_getPeriod(void)
{
    return RTC.PER;
}

//Returns the compare of the RTC
uint16_t RTC_getCompare(void)
{
    return RTC.CMP;
}

void RTC_setPeriod(uint16_t period)
{
    while (RTC.STATUS & RTC_CTRLABUSY_bm);
    
    //Turn off the RTC
    RTC.CTRLA &= ~(RTC_RTCEN_bm);
    
    while (RTC.STATUS & RTC_CNTBUSY_bm);
    
    //Clear Counter
    RTC.CNT = 0x0000;
    
    while (RTC.STATUS & RTC_PERBUSY_bm);
    
    //Write Value
    RTC.PER = period;
 
    while (RTC.STATUS & RTC_CTRLABUSY_bm);
    
    //Reset the RTC
    RTC.CTRLA |= RTC_RTCEN_bm;
}

//Sets the compare of the RTC
void RTC_setCompare(uint16_t comp)
{
    while (RTC.STATUS & RTC_CTRLABUSY_bm);
    
    //Turn off the RTC
    RTC.CTRLA &= ~(RTC_RTCEN_bm);
    
    while (RTC.STATUS & RTC_CNTBUSY_bm);
    
    //Clear Counter
    RTC.CNT = 0x0000;
    
    while (RTC.STATUS & RTC_CMPBUSY_bm);
    
    //Write Value
    RTC.CMP = comp;
 
    while (RTC.STATUS & RTC_CTRLABUSY_bm);
    
    //Reset the RTC
    RTC.CTRLA |= RTC_RTCEN_bm;
}

//Returns true if PIT was triggered
bool RTC_isPITTriggered(void)
{
    cli();
    bool temp = RTC_PIT_triggered;
    sei();
    
    return temp;
}

//Clears PIT Triggered Flag
void RTC_clearPITTriggered(void)
{
    cli();
    RTC_PIT_triggered = false;
    sei();
}

//Returns true if RTC OVF was triggered
bool RTC_isOVFTriggered(void)
{
    cli();
    bool temp = RTC_OVF_triggered;
    sei();
    
    return temp;
}

//Clears RTC OVF Triggered Flag
void RTC_clearOVFTrigger(void)
{
    cli();
    RTC_OVF_triggered = false;
    sei();
}

//Returns true if RTC was triggered
bool RTC_isCMPTriggered(void)
{
    cli();
    bool temp = RTC_CMP_triggered;
    sei();
    
    return temp;
}

//Clears RTC Triggered Flag
void RTC_clearCMPTrigger(void)
{
    cli();
    RTC_CMP_triggered = false;
    sei();
}


ISR(RTC_CNT_vect)
{
    if (RTC.INTFLAGS & RTC_OVF_bm)
    {
        //Overflow Flag
    
        //Set RTC Trigger Flag
        RTC_OVF_triggered = true;
        
        //If callback is set...
        if (RTC_OVF_handler)
        {
            RTC_OVF_handler();
        }
        
        //Clear OVF Flag
        RTC.INTFLAGS |= RTC_OVF_bm;
    }
    if (RTC.INTFLAGS & RTC_CMP_bm)
    {
        //Compare Flag
        RTC_CMP_triggered = true;
        
        //If callback is set...
        if (RTC_CMP_handler)
        {
            RTC_CMP_handler();
        }
        
        //Clear OVF Flag
        RTC.INTFLAGS |= RTC_CMP_bm;

    }
    
}

ISR(RTC_PIT_vect)
{
    //Set PIT Trigger Flag
    RTC_PIT_triggered = true;
    
    //If callback is set...
    if (PIT_handler)
    {
        PIT_handler();
    }
    
    //Clear PIT Interrupt
    RTC.PITINTFLAGS |= RTC_PI_bm;
}