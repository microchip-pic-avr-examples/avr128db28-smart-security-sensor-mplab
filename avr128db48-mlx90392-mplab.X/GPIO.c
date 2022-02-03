#include "GPIO.h"

#include <avr/io.h>

//Inits. General Purpose I/O
void GPIO_init(void)
{
    //PB2 - SW0 Input
    //Inverted Logic, Enable Pull-Up
    PORTB.PIN2CTRL = PORT_INVEN_bm | PORT_PULLUPEN_bm;
    
    //PB3 - LED0 Output
    //Output, Start LOW
    PORTB.DIRSET = PIN3_bm;
    
    //PD0 - RN4870_MODE Output (Start High)
    //PD2 - Debug GPIO
    
    PORTD.OUTSET = PIN0_bm;
    PORTD.DIRSET = PIN0_bm | PIN2_bm;
}
