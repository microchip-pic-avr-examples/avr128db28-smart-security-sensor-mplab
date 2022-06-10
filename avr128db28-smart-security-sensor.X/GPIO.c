#include "GPIO.h"

#include <avr/io.h>
#include <avr/interrupt.h>

//Inits. General Purpose I/O
void GPIO_init(void)
{    
    //PA0 - SW0 Input (Wake / Calibrate)
    //PA1 - BTLE_STS (Input)    
    //PA4, PA4 - MCU_TXD/MCU_RXD (For Bluetooth)
    //PA6 - Voltage Divider Enable

    //IO Set by USART initializer
    
    //Disable Inputs of Pins (PORTA)
    PORTA.PINCONFIG = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PINCTRLSET = PIN2_bm | PIN3_bm | PIN4_bm | PIN6_bm | PIN7_bm;
    
    //Pin 6 is an Output
    PORTA.DIRSET = PIN6_bm;
    
    //PC2, PC3 - MVIO I2C
    //IO Set by TWI Initializers
        
    //Disable Inputs of Pins (PORTC)
    PORTC.PINCONFIG = PORT_ISC_INPUT_DISABLE_gc;
    PORTC.PINCTRLUPD = PIN7_bm | PIN6_bm | PIN5_bm | PIN4_bm | PIN1_bm | PIN0_bm;;
    PORTC.DIRSET = PIN1_bm | PIN0_bm;

    //PD0 - <Not a Pin> Errata Fix
    //PD1 - BTLE Power Gate
    //PD2 - BTLE Reset_n
    //PD3, PD4, PD5 - LED0 Outputs
    //PD6 - VBAT
    
    //Outputs
    PORTD.DIRSET = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm | PIN7_bm;
        
    //Invert Outputs of LEDs
    PORTD.PINCONFIG = PORT_INVEN_bm | PORT_ISC_INPUT_DISABLE_gc;
    PORTD.PINCTRLSET = PIN3_bm | PIN4_bm | PIN5_bm;
    
    //Disable Inputs of Pins (PORTD)
    PORTD.PINCONFIG = PORT_ISC_INPUT_DISABLE_gc;
    PORTD.PINCTRLSET = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;
        
    //PF0, PF1 - DBG_TXD, DBG_RXD 
    //PF6 - Reset (Fuse Set)
    //IO Set by USART initializer
    PORTF.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTF.PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc;
}