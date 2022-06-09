#include "system.h"

#include <avr/io.h>

#include "GPIO.h"
#include "MVIO.h"
#include "TCB0_oneShot.h"
#include "TWI0_host.h"
#include "usart0.h"
#include "usart2.h"
#include "RTC.h"
#include "ADC.h"
#include "TCA0.h"

FUSES = 
{
  .BODCFG = ACTIVE_DISABLE_gc | LVL_BODLEVEL0_gc | SAMPFREQ_32Hz_gc | SLEEP_DISABLE_gc,
  .BOOTSIZE = 0x0,
  .CODESIZE = 0x0,
  .OSCCFG = CLKSEL_OSCHF_gc,
  .SYSCFG0 = CRCSEL_CRC16_gc | CRCSRC_NOCRC_gc | RSTPINCFG_RST_gc,
  .SYSCFG1 = MVSYSCFG_DUAL_gc | SUT_64MS_gc,
  .WDTCFG = PERIOD_1KCLK_gc | WINDOW_OFF_gc,  //PERIOD_1K
};

//Inits CLKCTRL, EVSYS, WDT, and SLPCTRL
void System_initDevice(void)
{
    //Init CLKCTRL
    //Default is 4MHz, which is desired
    
    //Init WDT
    
    
    //Init SLPCTRL
    
    //Power Level = STANDBY, SLEEP ENABLED
    SLPCTRL.CTRLA = SLPCTRL_SMODE_STDBY_gc | SLPCTRL_SEN_bm;
}

//Inits peripherals
void System_initPeripherals(void)
{    
    //Init GPIO
    GPIO_init();
    
    //Init MVIO
    MVIO_init();
    
    //Init EVSYS
    //TCB0 Uses Channel 0 (TCB sets up events)
    
    //Configure TWI0 (MVIO)
    TWI0_initPins();
    TWI0_initHost();
    
    //Configure USART 2 for BLE
    USART0_init();
    //(USART0_initIO called later)
    
    //Configure USART 3 for USB
    USART2_init();
    USART2_initIO();
    
    //Enable USART for USB (TX Only)
    //USART2_enableTX();
    
    //Init TCB0 for 1-shot timing
    TCB0_init();
    TCB0_enable();
    
    //Init RTC
    RTC_init();
    
    //Init ADC
    ADC_init();
    
    //Init TCA
    TCA0_init();
    TCA0_initIO();
}