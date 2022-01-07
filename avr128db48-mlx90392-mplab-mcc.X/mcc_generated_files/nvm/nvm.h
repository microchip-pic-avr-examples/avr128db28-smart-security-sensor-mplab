/**
 * 
 * @file nvm.h
 *
 * @defgroup nvm_driver  Non Volatile Memory
 *
 * @brief This file contains API prototypes and other datatypes for NVM driver.
 *
 * @version NVM Driver Version 2.0.0
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

#ifndef NVM_H
#define NVM_H

#include <string.h>
#include "../system/utils/compiler.h"
#include "../system/ccp.h"


/**
 * @ingroup nvm_driver
 * @brief Datatypes for flash address and data
 */
typedef uint8_t eeprom_data_t;
typedef uint16_t eeprom_adr_t;

/**
 * @ingroup nvm_driver
 * @brief This API initializes the NVM driver.
 *        This API must be called before any other Flash or EEPROM APIs.
 * @param void
 * @return void
 *
 * @code
 * void main(void)
 * {
 *     SYSTEM_Initialize();
 *     NVM_Initialize();
 *
 *     // User can call any Flash or EEPROM APIs here
 *
 *     while(1)
 *     {
 *     }
 * }
 * @endcode
 */
void NVM_Initialize(void);


/**
 * @ingroup nvm_driver
 * @brief This API checks if EEPROM is busy.
 *        NVM must be initialized with @ref NVM_Initialize() before calling this API.
 * @param void
 * @return true - if EEPROM is busy; false - if EEPROM is free
 *
 * @code
 * void main(void)
 * {
 *     SYSTEM_Initialize();
 *     NVM_Initialize();
 *
 *     if(!EEPROM_IsBusy())
 *     {
 *         // User can access EEPROM here
 *     }
 *
 *     while(1)
 *     {
 *     }
 * }
 * @endcode
 */
bool EEPROM_IsBusy(void);

/**
 * @ingroup nvm_driver
 * @brief This API reads one byte from given EEPROM address.
 *        NVM must be initialized with @ref NVM_Initialize() before calling this API.
 * @param [in] address - Address of EEPROM location to be read
 * @return Byte read from given EEPROM address
 *
 * @code
 * void main(void)
 * {
 *     data_t eeByte;
 *
 *     SYSTEM_Initialize();
 *     NVM_Initialize();
 *
 *     eeByte = EEPROM_Read(address);
 *
 *     while(1)
 *     {
 *     }
 * }
 * @endcode
 */
eeprom_data_t EEPROM_Read(eeprom_adr_t address);

/**
 * @ingroup nvm_driver
 * @brief This API writes one byte to given EEPROM address
 * @param [in] address - Address of EEPROM location to be written
 * @param [in] data - Byte to be written to given EEPROM location
 * @param [in] address - Starting address of Flash page to be erased
 * @return None
 *
 * @code
 * void main(void)
 * {
 *     data_t eeByte;
 *
 *     SYSTEM_Initialize();
 *     NVM_Initialize();
 *
 *     if(!EEPROM_IsBusy())
 *     {
 *         EEPROM_Write(address, 0xAA);
 *     }
 *
 *     while(1)
 *     {
 *     }
 * }
 * @endcode
 */
void EEPROM_Write(eeprom_adr_t address, eeprom_data_t data);


/**
 * @ingroup nvm_driver
 * @brief This function performs tasks to be executed on EEPROM Ready Interrupt
 * @param void
 * @return void
 *
 * @code
 * void main(void)
 * {
 *     SYSTEM_Initialize();
 *     NVM_Initialize();
 *
 *     while(1)
 *     {
 *         EEPROM_Task();
 *     }
 * }
 * @endcode
 */
void EEPROM_Task(void);

/**
 * @ingroup nvm_driver
 * @brief Setter function for EEPROM Ready interrupt callback
 * @param CallbackHandler - Pointer to custom Callback.
 * @return void
 *
 * @code
 * void customEEReadyCallback(void)
 * {
 *    // Custom ISR code
 * }
 *
 * void main(void)
 * {
 *     SYSTEM_Initialize();
 *     NVM_Initialize();
 *     EEPROM_CallbackRegister(customEEReadyCallback);
 *
 *     while(1)
 *     {
 *     }
 * }
 * @endcode
 */
void EEPROM_CallbackRegister(void (* CallbackHandler)(void));

#endif //NVM_H
