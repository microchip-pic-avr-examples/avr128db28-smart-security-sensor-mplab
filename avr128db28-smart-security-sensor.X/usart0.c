#include "usart0.h"
#include "GPIO.h"

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

static void (*rxHandler)(char);
static volatile bool isRXBusy = false;

void USART0_init(void)
{
    //Init Callback
    rxHandler = 0;
    
    //Enable Run in Debug
    USART0.DBGCTRL = USART_DBGRUN_bm;
    
    //Enable RX Interrupts
    USART0.CTRLA = USART_RXCIE_bm;
    
    //Async Mode, No Parity, 1 Stop Bit, 8 bit TX
    USART0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc | USART_CHSIZE_8BIT_gc;
    
    //Baud for 115,200 at 4 MHz Clock
    USART0.BAUD = 139;
    
    //Enable Normal Mode for USART
    USART0.CTRLB = USART_RXMODE_NORMAL_gc;
    
    //Note: Call enableTX/enableRX after this function
}

void USART0_initIO(void)
{
    //Default Value for output should be 1
    PORTA.OUTSET = PIN4_bm;
    
    //Clear PORTMUX for USART0
    PORTMUX.USARTROUTEA &= ~(PORTMUX_USART0_gm);
    
    //Set PORTMUX for USART0
    PORTMUX.USARTROUTEA |= (PORTMUX_USART0_ALT1_gc);
    
    //Set PA4 as Output
    PORTA.DIRSET = PIN4_bm;
    
    //Set PA5 as Input
    PORTA.DIRCLR = PIN5_bm;
}

//Enable TX 
void USART0_enableTX(void)
{
    USART0.CTRLB |= USART_TXEN_bm;
}

//Disable TX
void USART0_disableTX(void)
{
    PORTA.OUTCLR = PIN4_bm;
    USART0.CTRLB &= ~(USART_TXEN_bm);
}

//Enable RX
void USART0_enableRX(void)
{
    USART0.CTRLB |= USART_RXEN_bm;
}

//Disable RX
void USART0_disableRX(void)
{
    USART0.CTRLB &= ~(USART_RXEN_bm);
}

//Sets the callback function for Data RX
void USART0_setRXCallback(void (*function)(char))
{
    rxHandler = function;
}

//Sends a byte of data
bool USART0_sendByte(char c)
{
    if (USART0_canTransmit())
    {
        //Clear done bit
        USART0.STATUS |= USART_TXCIF_bm;

        //Load Byte of Data
        USART0.TXDATAL = c;
        return true;
    }
    return false;
}

//Returns true if the peripheral can accept a new byte
bool USART0_canTransmit(void)
{
    return (USART0.STATUS & USART_DREIF_bm);
}

//Returns true if the shifter is running
bool USART0_isBusy(void)
{
    return (!(USART0.STATUS & USART_TXCIF_bm));
}

//Returns true if data is being shifted in
bool USART0_isRXActive(void)
{
    return isRXBusy;
}

ISR(USART0_RXC_vect)
{
    //Data    
    char rx = USART0.RXDATAL;
    if (rxHandler)
    {
        rxHandler(rx);
    }
    isRXBusy = false;
}