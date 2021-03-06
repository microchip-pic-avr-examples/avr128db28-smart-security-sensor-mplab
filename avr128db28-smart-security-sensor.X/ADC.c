#include "ADC.h"

#include <avr/io.h>

#include <stdint.h>

//Init the ADC
void ADC_init(void)
{
    //Enable run in debug
    ADC0.DBGCTRL = ADC_DBGRUN_bm;
    
    //Single Ended Mode, 10-bit Resolution
    ADC0.CTRLA = ADC_CONVMODE_SINGLEENDED_gc | ADC_RESSEL_10BIT_gc;
    
    //VDD Reference
    VREF.ADC0REF = VREF_REFSEL_VDD_gc;
    
    //Divide the Peripheral Clock is 128 (4 MHz / 128 = 31.25 kHz)
    ADC0.CTRLC = ADC_PRESC_DIV128_gc;
    
    //Extra Clock Cycles to Hold
    //~800us for 5k from the divider
    //~120us for turn on time
    //Total: ~920us
    ADC0.SAMPCTRL = 29;
            
    //Setup Sampling Channels
    ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc;
    ADC0.MUXNEG = ADC_MUXNEG_GND_gc;
}

//Enables the ADC
void ADC_enable(void)
{
    //Enable the ADC
    ADC0.CTRLA |= ADC_ENABLE_bm;

}

//Disables the ADC (to save power)
void ADC_disable(void)
{
    //Enable the ADC
    ADC0.CTRLA &= ~ADC_ENABLE_bm;
}

//Trigger a conversion and print the value
uint16_t ADC_getValue(uint8_t channel)
{
    ADC0.MUXPOS = channel;
    
    //Start Conversion
    ADC0.COMMAND = ADC_STCONV_bm;
    
    //While Converting...
    while (ADC0.COMMAND & ADC_STCONV_bm) { ; }
    
    //Return Result
    return ADC0.RES;
}

//Trigger a conversion and return it in float
float ADC_getResultAsFloat(uint8_t channel)
{
    //Get Raw Value
    uint16_t val = ADC_getValue(channel);
    
    //Convert to Float
    //3.3 /  1024 = Vbits
    //Vbits * val = voltage / 2
    
    //(3.3 * 2) / 1024 = 0.006445mV per bit
    float result = 0.006445 * val;
    
    return result;
}