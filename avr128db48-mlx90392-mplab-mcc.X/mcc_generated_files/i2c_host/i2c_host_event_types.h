/**
 * \file
 * i2c_host_event_types.h
 *
 * \defgroup
 * i2c_host_events
 *
 * \brief
 * This file contains API prototypes and other data types for I2C driver.
 *
 * \version
 * I2C Host Driver Version 2.0.0
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

#ifndef I2C_HOST_EVENT_TYPES_H
#define	I2C_HOST_EVENT_TYPES_H

#include "i2c_host_types.h"
/**
 * \brief This file contains API prototypes and data structures for I2C Host driver.
 * \ingroup i2c_host_events  I2C Host Driver
 * \{
 */

/**
 * \ingroup i2c_host_events
 * \brief
 *       I2C STATES
 */
typedef enum
{
    I2C_STATE_IDLE = 0,
    I2C_STATE_SEND_RD_ADDR,
    I2C_STATE_SEND_WR_ADDR,
    I2C_STATE_TX,
    I2C_STATE_RX,
    I2C_STATE_NACK,
    I2C_STATE_ERROR,
    I2C_STATE_STOP,
    I2C_STATE_RESET
} i2c_event_states_t;

/**
 * \ingroup i2c_host_events
 * \brief
 *       I2C Event Status Structure
 */
typedef struct
{
    bool busy;
    uint16_t address;
    uint8_t *writePtr;
    size_t writeLength;
    uint8_t *readPtr;
    size_t readLength;
    bool switchToRead;
    i2c_host_error_t errorState; 
    i2c_event_states_t state;
} i2c_event_status_t;
/**
 * \}
 */
#endif // end of I2C_HOST_EVENT_TYPES_H