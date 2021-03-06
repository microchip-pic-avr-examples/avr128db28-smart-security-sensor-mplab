#include "usart2.h"
#include "GPIO.h"

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

static void (*rxHandler)(char);
static volatile bool isRXBusy = false;

void USART2_init(void)
{
    //Init Callback
    rxHandler = 0;
    
    //Enable Run in Debug
    USART2.DBGCTRL = USART_DBGRUN_bm;
    
    //Enable RX Interrupts, Start of Frame
    USART2.CTRLA = USART_RXCIE_bm | USART_RXSIE_bm;
    
    //Async Mode, No Parity, 1 Stop Bit, 8 bit TX
    USART2.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc | USART_CHSIZE_8BIT_gc;
    
    //Baud for 115,200 at 4 MHz Clock
    USART2.BAUD = 139;
    
    //Enable Normal Mode for USART, Enable Start of Frame Detect
    USART2.CTRLB = USART_RXMODE_NORMAL_gc;
    
    //Note: Call enableTX/enableRX after this function
}

void USART2_initIO(void)
{
    //Default Value for output should be 1
    PORTF.OUTSET = PIN0_bm;
    
    //Set PB0 as Output
    PORTF.DIRSET = PIN0_bm;
    
    //Set PB1 as Input
    PORTF.DIRCLR = PIN1_bm;
}

//Enable TX 
void USART2_enableTX(void)
{
    USART2.CTRLB |= USART_TXEN_bm;
}

//Disable TX
void USART2_disableTX(void)
{
    USART2.CTRLB &= ~(USART_TXEN_bm);
    
    //Default Value for output should be 1
    PORTF.OUTSET = PIN0_bm;
    
    //Set PB0 as Output
    PORTF.DIRSET = PIN0_bm;
}

//Enable RX
void USART2_enableRX(void)
{
    USART2.CTRLB |= USART_RXEN_bm;
}

//Disable RX
void USART2_disableRX(void)
{
    USART2.CTRLB &= ~(USART_RXEN_bm);
}

//Sets the callback function for Data RX
void USART2_setRXCallback(void (*function)(char))
{
    rxHandler = function;
}

//Sends a byte of data
bool USART2_sendByte(char c)
{
    if (USART2_canTransmit())
    {
        //Clear done bit
        USART2.STATUS |= USART_TXCIF_bm;

        //Load Byte of Data
        USART2.TXDATAL = c;
        return true;
    }
    return false;
}

//Returns true if the peripheral can accept a new byte
bool USART2_canTransmit(void)
{
    return (USART2.STATUS & USART_DREIF_bm);
}

//Returns true if the shifter is running
bool USART2_isBusy(void)
{
    return (!(USART2.STATUS & USART_TXCIF_bm));
}

//Returns true if data is being shifted in
bool USART2_isRXActive(void)
{
    return isRXBusy;
}

ISR(USART2_RXC_vect)
{
    char rx = USART2.RXDATAL;
    if (rxHandler)
    {
        rxHandler(rx);
    }
    isRXBusy = false;
}