#include "system.h"

#include <avr/io.h>

#include "GPIO.h"
#include "MVIO.h"
#include "TCB0_oneShot.h"
#include "TWI0_host.h"
#include "TWI1_host.h"
#include "usart2.h"
#include "usart3.h"
#include "RTC.h"

FUSES = 
{
  .BODCFG = ACTIVE_DISABLE_gc | LVL_BODLEVEL0_gc | SAMPFREQ_128Hz_gc | SLEEP_DISABLE_gc,
  .BOOTSIZE = 0x0,
  .CODESIZE = 0x0,
  .OSCCFG = CLKSEL_OSCHF_gc,
  .SYSCFG0 = CRCSEL_CRC16_gc | CRCSRC_NOCRC_gc | RSTPINCFG_GPIO_gc,
  .SYSCFG1 = MVSYSCFG_DUAL_gc | SUT_16MS_gc,
  .WDTCFG = PERIOD_1KCLK_gc | WINDOW_OFF_gc,
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
    //Init EVSYS
    
    //TCB0 uses channel 0
    EVSYS.USERTCB0CAPT = EVSYS_USER_CHANNEL0_gc;
    
    //Init GPIO
    GPIO_init();
    
    //Init MVIO
    MVIO_init();
        
    //Configure TWI0 (MVIO) for Magnetometer
    TWI0_initHost();
    TWI0_initPins();
    
    //Configure TWI1 for Thermometer
    TWI1_initHost();
    TWI1_initPins();
    
    //Configure USART 2 for BLE
    USART2_init();
    USART2_initIO();
    
    //Configure USART 3 for USB
    USART3_init();
    USART3_initIO();
        
    //Enable USART for BLE
    USART2_enableRX();
    USART2_enableTX();
    
    //Enable USART for USB (TX Only)
    USART3_enableTX();
    
    //Init TCB0 for 1-shot timing
    TCB0_init();
    
    //Init RTC
    RTC_init();
}