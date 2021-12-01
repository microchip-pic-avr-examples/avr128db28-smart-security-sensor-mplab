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


#include "../usart3.h"
#define RECEIVE_ERROR_MASK 0x46


static void DefaultFramingErrorCallback(void);
static void DefaultOverrunErrorCallback(void);
static void DefaultParityErrorCallback(void);
void (*USART3_framing_err_cb)(void) = &DefaultFramingErrorCallback;
void (*USART3_overrun_err_cb)(void) = &DefaultOverrunErrorCallback;
void (*USART3_parity_err_cb)(void) = &DefaultParityErrorCallback;

const struct UART_INTERFACE USART3_Interface = {
  .Initialize = USART3_Initialize,
  .Write = USART3_Write,
  .Read = USART3_Read,
  .RxCompleteCallbackRegister = NULL,
  .TxCompleteCallbackRegister = NULL,
  .ErrorCallbackRegister = NULL,
  .FramingErrorCallbackRegister = USART3_FramingErrorCallbackRegister,
  .OverrunErrorCallbackRegister = USART3_OverrunErrorCallbackRegister,
  .ParityErrorCallbackRegister = USART3_ParityErrorCallbackRegister,
  .ChecksumErrorCallbackRegister = NULL,
  .IsRxReady = USART3_IsRxReady,
  .IsTxReady = USART3_IsTxReady,
  .IsTxDone = USART3_IsTxDone
};

void USART3_Initialize(void)
{
    //set baud rate register
    USART3.BAUD = (uint16_t)USART3_BAUD_RATE(115200);
	
    // ABEIE disabled; DREIE disabled; LBME disabled; RS485 DISABLE; RXCIE disabled; RXSIE disabled; TXCIE disabled; 
    USART3.CTRLA = 0x0;
	
    // MPCM disabled; ODME disabled; RXEN enabled; RXMODE NORMAL; SFDEN disabled; TXEN enabled; 
    USART3.CTRLB = 0xC0;
	
    // CMODE Asynchronous Mode; UCPHA enabled; UDORD disabled; CHSIZE Character size: 8 bit; PMODE No Parity; SBMODE 1 stop bit; 
    USART3.CTRLC = 0x3;
	
    //DBGCTRL_DBGRUN
    USART3.DBGCTRL = 0x0;
	
    //EVCTRL_IREI
    USART3.EVCTRL = 0x0;
	
    //RXPLCTRL_RXPL
    USART3.RXPLCTRL = 0x0;
	
    //TXPLCTRL_TXPL
    USART3.TXPLCTRL = 0x0;
	

}

void USART3_FramingErrorCallbackRegister(void* callbackHandler)
{
    USART3_framing_err_cb=callbackHandler;
}

void USART3_OverrunErrorCallbackRegister(void* callbackHandler)
{
    USART3_overrun_err_cb=callbackHandler;
}

void USART3_ParityErrorCallbackRegister(void* callbackHandler)
{
    USART3_parity_err_cb=callbackHandler;
}

static void DefaultFramingErrorCallback(void)
{
    /* Add your interrupt code here or use USART3.FramingCallbackRegister function to use Custom ISR */
}

static void DefaultOverrunErrorCallback(void)
{
   /* Add your interrupt code here or use USART3.OverrunCallbackRegister function to use Custom ISR */
}

static void DefaultParityErrorCallback(void)
{
    /* Add your interrupt code here or use USART3.ParityCallbackRegister function to use Custom ISR */
}

void USART3_Enable(void)
{
    USART3.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
}

void USART3_EnableRx(void)
{
    USART3.CTRLB |= USART_RXEN_bm;
}

void USART3_EnableTx(void)
{
    USART3.CTRLB |= USART_TXEN_bm;
}

void USART3_Disable(void)
{
    USART3.CTRLB &= ~(USART_RXEN_bm | USART_TXEN_bm);
}

uint8_t USART3_GetData(void)
{
    return USART3.RXDATAL;
}

bool USART3_IsTxReady(void)
{
    return (USART3.STATUS & USART_DREIF_bm);
}

bool USART3_IsRxReady(void)
{
    return (USART3.STATUS & USART_RXCIF_bm);
}

bool USART3_IsTxBusy(void)
{
    return (!(USART3.STATUS & USART_TXCIF_bm));
}

bool USART3_IsTxDone(void)
{
    return (USART3.STATUS & USART_TXCIF_bm);
}

void USART3_ErrorCheck(void)
{
    uint8_t errorMask = USART3.RXDATAH;
    if(errorMask & RECEIVE_ERROR_MASK)
    {
        if(errorMask & USART_PERR_bm)
        {
            USART3_parity_err_cb();
        } 
        if(errorMask & USART_FERR_bm)
        {
            USART3_framing_err_cb();
        }
        if(errorMask & USART_BUFOVF_bm)
        {
            USART3_overrun_err_cb();
        }
    }
    
}

uint8_t USART3_Read(void)
{       
    return USART3.RXDATAL;
}

void USART3_Write(const uint8_t data)
{
    USART3.TXDATAL = data;
}