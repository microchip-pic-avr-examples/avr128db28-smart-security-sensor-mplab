#include "GPIO.h"

#include <avr/io.h>

//Inits. General Purpose I/O
void GPIO_init(void)
{    
    //PA0 - SW0 Input (Wake / Calibrate)
    //PA1 - BTLE_STS (Input)
    
    //PA4, PA4 - MCU_TXD/MCU_RXD (For Bluetooth)
    //IO Set by USART initializer
    
    //PC2, PC3 - MVIO I2C
    //IO Set by TWI Initializers
    
    //PD1 - BTLE Power Gate
    //PD2 - BTLE Reset_n
    //PD3, PD4, PD5 - LED0 Outputs
    
    //Outputs
    PORTD.DIRSET = PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm;
    
    //Invert Outputs of LEDs
    PORTD.PINCONFIG = PORT_INVEN_bm;
    PORTD.PINCTRLSET = PIN3_bm | PIN4_bm | PIN5_bm;
    
    //PD6 - VBAT
    //Analog Input, Disable Digital Input Buffers
    PORTD.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc;
        
    //PF0, PF1 - DBG_TXD, DBG_RXD 
    //IO Set by USART initializer
}