/**
  @Company
    Microchip Technology Inc.

  @Description
    This Source file provides APIs.
    Generation Information :
    Driver Version    :   1.0.0
*/
/*
© [2021] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/


#include "../usart2.h"
#define RECEIVE_ERROR_MASK 0x46


static void DefaultFramingErrorCallback(void);
static void DefaultOverrunErrorCallback(void);
static void DefaultParityErrorCallback(void);
void (*USART2_framing_err_cb)(void) = &DefaultFramingErrorCallback;
void (*USART2_overrun_err_cb)(void) = &DefaultOverrunErrorCallback;
void (*USART2_parity_err_cb)(void) = &DefaultParityErrorCallback;

const struct UART_INTERFACE USART2_Interface = {
  .Initialize = USART2_Initialize,
  .Write = USART2_Write,
  .Read = USART2_Read,
  .RxCompleteCallbackRegister = NULL,
  .TxCompleteCallbackRegister = NULL,
  .ErrorCallbackRegister = NULL,
  .FramingErrorCallbackRegister = USART2_FramingErrorCallbackRegister,
  .OverrunErrorCallbackRegister = USART2_OverrunErrorCallbackRegister,
  .ParityErrorCallbackRegister = USART2_ParityErrorCallbackRegister,
  .ChecksumErrorCallbackRegister = NULL,
  .IsRxReady = USART2_IsRxReady,
  .IsTxReady = USART2_IsTxReady,
  .IsTxDone = USART2_IsTxDone
};

void USART2_Initialize(void)
{
    //set baud rate register
    USART2.BAUD = (uint16_t)USART2_BAUD_RATE(115200);
	
    // ABEIE disabled; DREIE disabled; LBME disabled; RS485 DISABLE; RXCIE disabled; RXSIE disabled; TXCIE disabled; 
    USART2.CTRLA = 0x0;
	
    // MPCM disabled; ODME disabled; RXEN enabled; RXMODE NORMAL; SFDEN disabled; TXEN enabled; 
    USART2.CTRLB = 0xC0;
	
    // CMODE Asynchronous Mode; UCPHA enabled; UDORD disabled; CHSIZE Character size: 8 bit; PMODE No Parity; SBMODE 1 stop bit; 
    USART2.CTRLC = 0x3;
	
    //DBGCTRL_DBGRUN
    USART2.DBGCTRL = 0x0;
	
    //EVCTRL_IREI
    USART2.EVCTRL = 0x0;
	
    //RXPLCTRL_RXPL
    USART2.RXPLCTRL = 0x0;
	
    //TXPLCTRL_TXPL
    USART2.TXPLCTRL = 0x0;
	

}

void USART2_FramingErrorCallbackRegister(void* callbackHandler)
{
    USART2_framing_err_cb=callbackHandler;
}

void USART2_OverrunErrorCallbackRegister(void* callbackHandler)
{
    USART2_overrun_err_cb=callbackHandler;
}

void USART2_ParityErrorCallbackRegister(void* callbackHandler)
{
    USART2_parity_err_cb=callbackHandler;
}

static void DefaultFramingErrorCallback(void)
{
    /* Add your interrupt code here or use USART2.FramingCallbackRegister function to use Custom ISR */
}

static void DefaultOverrunErrorCallback(void)
{
   /* Add your interrupt code here or use USART2.OverrunCallbackRegister function to use Custom ISR */
}

static void DefaultParityErrorCallback(void)
{
    /* Add your interrupt code here or use USART2.ParityCallbackRegister function to use Custom ISR */
}

void USART2_Enable(void)
{
    USART2.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
}

void USART2_EnableRx(void)
{
    USART2.CTRLB |= USART_RXEN_bm;
}

void USART2_EnableTx(void)
{
    USART2.CTRLB |= USART_TXEN_bm;
}

void USART2_Disable(void)
{
    USART2.CTRLB &= ~(USART_RXEN_bm | USART_TXEN_bm);
}

uint8_t USART2_GetData(void)
{
    return USART2.RXDATAL;
}

bool USART2_IsTxReady(void)
{
    return (USART2.STATUS & USART_DREIF_bm);
}

bool USART2_IsRxReady(void)
{
    return (USART2.STATUS & USART_RXCIF_bm);
}

bool USART2_IsTxBusy(void)
{
    return (!(USART2.STATUS & USART_TXCIF_bm));
}

bool USART2_IsTxDone(void)
{
    return (USART2.STATUS & USART_TXCIF_bm);
}

void USART2_ErrorCheck(void)
{
    uint8_t errorMask = USART2.RXDATAH;
    if(errorMask & RECEIVE_ERROR_MASK)
    {
        if(errorMask & USART_PERR_bm)
        {
            USART2_parity_err_cb();
        } 
        if(errorMask & USART_FERR_bm)
        {
            USART2_framing_err_cb();
        }
        if(errorMask & USART_BUFOVF_bm)
        {
            USART2_overrun_err_cb();
        }
    }
    
}

uint8_t USART2_Read(void)
{       
    return USART2.RXDATAL;
}

void USART2_Write(const uint8_t data)
{
    USART2.TXDATAL = data;
}