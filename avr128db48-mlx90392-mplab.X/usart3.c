#include "usart3.h"
#include "GPIO.h"

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

static void (*rxHandler)(char);
static volatile bool isRXBusy = false;

void USART3_init(void)
{
    //Init Callback
    rxHandler = 0;
    
    //Enable Run in Debug
    USART3.DBGCTRL = USART_DBGRUN_bm;
    
    //Enable RX Interrupts, Start of Frame
    USART3.CTRLA = USART_RXCIE_bm | USART_RXSIE_bm;
    
    //Async Mode, No Parity, 1 Stop Bit, 8 bit TX
    USART3.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc | USART_CHSIZE_8BIT_gc;
    
    //Baud for 115,200 at 4 MHz Clock
    USART3.BAUD = 139;
    
    //Enable Normal Mode for USART, Enable Start of Frame Detect
    USART3.CTRLB = USART_RXMODE_NORMAL_gc;
    
    //Note: Call enableTX/enableRX after this function
}

void USART3_initIO(void)
{
    //Default Value for output should be 1
    PORTB.OUTSET = PIN0_bm;
    
    //Set PB0 as Output
    PORTB.DIRSET = PIN0_bm;
    
    //Set PB1 as Input
    PORTB.DIRCLR = PIN1_bm;
}

//Enable TX 
void USART3_enableTX(void)
{
    USART3.CTRLB |= USART_TXEN_bm;
}

//Disable TX
void USART3_disableTX(void)
{
    USART3.CTRLB &= ~(USART_TXEN_bm);
}

//Enable RX
void USART3_enableRX(void)
{
    USART3.CTRLB |= USART_RXEN_bm;
}

//Disable RX
void USART3_disableRX(void)
{
    USART3.CTRLB &= ~(USART_RXEN_bm);
}

//Sets the callback function for Data RX
void USART3_setRXCallback(void (*function)(char))
{
    rxHandler = function;
}

//Sends a byte of data
bool USART3_sendByte(char c)
{
    if (USART3_canTransmit())
    {
        //Clear done bit
        USART3.STATUS |= USART_TXCIF_bm;

        //Load Byte of Data
        USART3.TXDATAL = c;
        return true;
    }
    return false;
}

//Returns true if the peripheral can accept a new byte
bool USART3_canTransmit(void)
{
    return (USART3.STATUS & USART_DREIF_bm);
}

//Returns true if the shifter is running
bool USART3_isBusy(void)
{
    return (!(USART3.STATUS & USART_TXCIF_bm));
}

//Returns true if data is being shifted in
bool USART3_isRXActive(void)
{
    return isRXBusy;
}

ISR(USART3_RXC_vect)
{
    char rx = USART3.RXDATAL;
    if (rxHandler)
    {
        rxHandler(rx);
    }
    isRXBusy = false;
}