#include <avr/io.h>

#include <stdbool.h>

#include "TCA0.h"

//Init the TCA Peripheral
void TCA0_init(void)
{    
    //Enable Split Mode
    TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;
    
    //500 Hz period
    TCA0.SPLIT.HPER = TCA_SPLIT_PERIOD;
    
    //10% Duty Cycle
    TCA0.SPLIT.HCMP0 = TCA_LEDR_ON_TIME;
    TCA0.SPLIT.HCMP1 = TCA_LEDG_ON_TIME;
    TCA0.SPLIT.HCMP2 = TCA_LEDB_ON_TIME;
    
    //Clock Divider 64, TCA Enabled
    TCA0.SPLIT.CTRLA = TCA_SPLIT_CLKSEL_DIV256_gc;
}

//Init TCA IO
void TCA0_initIO(void)
{
    PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTD_gc;
}

//Enables TCA0
void TCA0_enable(void)
{
    TCA0.SPLIT.CTRLA |= TCA_SPLIT_ENABLE_bm;
}

//Disables TCA0
void TCA0_disable(void)
{
    TCA0.SPLIT.CTRLA &= ~TCA_SPLIT_ENABLE_bm;
}

//Enable CMP Outputs
void TCA0_enableHCMP0(void)
{
    TCA0.SPLIT.CTRLB |= TCA_SPLIT_HCMP0EN_bm;
}

void TCA0_enableHCMP1(void)
{
    TCA0.SPLIT.CTRLB |= TCA_SPLIT_HCMP1EN_bm;
}

void TCA0_enableHCMP2(void)
{
    TCA0.SPLIT.CTRLB |= TCA_SPLIT_HCMP2EN_bm;
}

//Disable CMP Outputs
void TCA0_disableHCMP0(void)
{
    TCA0.SPLIT.CTRLB &= ~(TCA_SPLIT_HCMP0EN_bm);
}

void TCA0_disableHCMP1(void)
{
    TCA0.SPLIT.CTRLB &= ~(TCA_SPLIT_HCMP1EN_bm);
}

void TCA0_disableHCMP2(void)
{
    TCA0.SPLIT.CTRLB &= ~(TCA_SPLIT_HCMP2EN_bm);
}

bool TCA0_getHCMP0EN(void)
{
    return (TCA0.SPLIT.CTRLB & TCA_SPLIT_HCMP0EN_bm);
}

bool TCA0_getHCMP1EN(void)
{
    return (TCA0.SPLIT.CTRLB & TCA_SPLIT_HCMP1EN_bm);
}

bool TCA0_getHCMP2EN(void)
{
    return (TCA0.SPLIT.CTRLB & TCA_SPLIT_HCMP2EN_bm);
}