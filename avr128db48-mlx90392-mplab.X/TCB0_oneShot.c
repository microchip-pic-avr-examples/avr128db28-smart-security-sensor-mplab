#include "TCB0_oneShot.h"

#include <stdbool.h>
#include <avr/io.h>

void TCB0_init(void)
{
    //Enable Debug Mode Operation
    TCB0.DBGCTRL = TCB_DBGRUN_bm;
    
    //Single Shot Mode
    TCB0.CTRLB = TCB_CNTMODE_SINGLE_gc;
    
    //Enable Capture Event Inputs
    TCB0.EVCTRL = TCB_CAPTEI_bm;
    
    //1ms Period @ 4 MHz
    //0x0FA0 = 4000 pulses
    TCB0.CCMP = 0x0FA0;
 
    //Select CLK_PER for clock, Enable TCB
    TCB0.CTRLA = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm;
}

//Returns true if TCB is currently running
bool TCB0_isRunning(void)
{
    return (TCB0.STATUS & TCB_RUN_bm);
}

void TCB0_triggerTimer(void)
{
    //Ensures timing requirements
    asm("NOP");
    asm("NOP");

    //Retrigger the Timer
    EVSYS.SWEVENTA = EVSYS_SWEVENTA_CH0_gc;
}