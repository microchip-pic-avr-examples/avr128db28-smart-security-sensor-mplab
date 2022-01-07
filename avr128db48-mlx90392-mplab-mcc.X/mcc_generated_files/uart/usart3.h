/**
  @Company
    Microchip Technology Inc.

  @Description
    This Source file provides APIs.
    Generation Information :
    Driver Version    :   1.0.0
*/
/*
© [2022] Microchip Technology Inc. and its subsidiaries.

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

#ifndef USART3_H
#define USART3_H

#include <stdbool.h>
#include <stdio.h>
#include "../system/utils/compiler.h"
#include "../system/utils/atomic.h"
#include "../system/clock.h"
#include "uart_interface.h"

/* Normal Mode, Baud register value */
#define USART3_BAUD_RATE(BAUD_RATE) (((float)4000000 * 64 / (16 * (float)BAUD_RATE)) + 0.5)

extern const struct UART_INTERFACE USART3_Interface;

/**
 * \brief Initialize USART interface
 * If module is configured to disabled state, the clock to the USART is disabled
 * if this is supported by the device's clock system.
 *
 * \return Initialization status.
 * \retval 0 the USART init was successful
 * \retval 1 the USART init was not successful
 */
void USART3_Initialize(void);

/**
 * \brief Enable RX and TX in USART3
 * 1. If supported by the clock system, enables the clock to the USART
 * 2. Enables the USART module by setting the RX and TX enable-bits in the USART control register
 *
 * \return Nothing
 */
void USART3_Enable(void);

/**
 * \brief Enable RX in USART3
 * 1. If supported by the clock system, enables the clock to the USART
 * 2. Enables the USART module by setting the RX enable-bit in the USART control register
 *
 * \return Nothing
 */
void USART3_EnableRx(void);

/**
 * \brief Enable TX in USART3
 * 1. If supported by the clock system, enables the clock to the USART
 * 2. Enables the USART module by setting the TX enable-bit in the USART control register
 *
 * \return Nothing
 */
void USART3_EnableTx(void);

/**
 * \brief Disable USART3
 * 1. Disables the USART module by clearing the enable-bit(s) in the USART control register
 * 2. If supported by the clock system, disables the clock to the USART
 *
 * \return Nothing
 */
void USART3_Disable(void);

/**
 * \brief Get recieved data from USART3
 *
 * \return Data register from USART3 module
 */
uint8_t USART3_GetData(void);

/**
 * \brief Check if the usart can accept data to be transmitted
 *
 * \return The status of USART TX data ready check
 * \retval false The USART can not receive data to be transmitted
 * \retval true The USART can receive data to be transmitted
 */
bool USART3_IsTxReady(void);

/**
 * \brief Check if the USART has received data
 *
 * \return The status of USART RX data ready check
 * \retval true The USART has received data
 * \retval false The USART has not received data
 */
bool USART3_IsRxReady(void);

/**
 * \brief Check if USART3 data is transmitted
 *
 * \return Receiver ready status
 * \retval true  Data is not completely shifted out of the shift register
 * \retval false Data completely shifted out if the USART shift register
 */
bool USART3_IsTxBusy(void);


bool USART3_IsTxDone(void);
/**
 * \brief Read one character from USART3
 *
 * Function will block if a character is not available.
 *
 * \return Data read from the USART3 module
 */
uint8_t USART3_Read(void);

/**
 * \brief Write one character to USART3
 *
 * Function will block until a character can be accepted.
 *
 * \param[in] data The character to write to the USART
 *
 * \return Nothing
 */
void USART3_Write(const uint8_t data);

/**
 * \brief ErrorCheck USART interface
 *
 * Checks the Recieve Error for Parity, Framing, OverRun
 *
 * No Return and Arguments associated
 */
void USART3_ErrorCheck(void);

void USART3_FramingErrorCallbackRegister(void* callbackHandler);

void USART3_OverrunErrorCallbackRegister(void* callbackHandler);

void USART3_ParityErrorCallbackRegister(void* callbackHandler);

#endif /* USART3_H */