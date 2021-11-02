/**
 * \file
 * i2c_host_interface.h
 *
 * \defgroup
 * i2c_host_interface
 *
 * \brief
 * This file contains API prototypes and other data types for i2c driver.
 *
 * \version
 * I2C Host Driver Version 2.0.0
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

#ifndef I2C_HOST_INTERFACE_H
#define I2C_HOST_INTERFACE_H
/**
  Section: Included Files
*/
#include <stdbool.h>
#include <stdint.h>
#include <xc.h>
#include "i2c_host_types.h"
    
/**
  I2C_HOST_INTERFACE

  @Description
    Structure containing the function pointers of I2C driver.
 */
typedef struct
{   
    void (*Initialize)(void);
    void (*Deinitialize)(void);
    bool (*Write)(uint16_t address, uint8_t *data, size_t dataLength);
    bool (*Read)(uint16_t address, uint8_t *data, size_t dataLength);
    bool (*WriteRead)(uint16_t address, uint8_t *writeData, size_t writeLength, uint8_t *readData, size_t readLength);
    bool (*TransferSetup)(struct I2C_TRANSFER_SETUP* setup, uint32_t srcClkFreq);
    i2c_host_error_t (*ErrorGet)(void);
    bool (*IsBusy)(void);
    void (*CallbackRegister)(void (*callback)(void));
    void (*Tasks)(void);
}i2c_host_interface_t;


/**
 * @ingroup i2c_host
 * @example i2c_host_interface_example_code
 * @brief In this I2C Host example, a byte of data is written in to EEPROM and read it back.
 *        Case 1: Data 0xAA is written in to EEPROM at location 0x0010 and read the same back, using I2C0_Write() and I2C0_WriteRead() functions.
 *        Case 2: Data 0x55 is written in to EEPROM at location 0x0020 and read the same back using I2C0_Write() and I2C0_Read() functions.
 * @code
 * 
 * void main(void)
 * {
 *    uint8_t transmitData[10];
 *    uint8_t writeLength;
 *    uint8_t readLength;
 *    uint8_t writeData;
 *    uint8_t readData;
 *    uint8_t eepromAddr = 0x50; // 7-bit EEPROM address
 *
 *    // Initializes Clock, Pins, Interrupts and I2C host
 *    SYSTEM_Initialize();
 *    // Case 1:
 *    transmitData[0] = 0x00;  // load MSB of EEPROM location
 *    transmitData[1] = 0x10;  // load LSB of EEPROM location
 *    transmitData[2] = 0xAA;  // load data
 *    writeLength = 3; // 2 bytes of location address + 1 byte data
 *
 *    if ( i2c1_host_interface.Write(eepromAddr, transmitData, writeLength))
 *    {
 *        while ( i2c1_host_interface.IsBusy())
 *        {
 *            i2c1_host_interface.Tasks();
 *        }
 *
 *        if (  i2c1_host_interface.ErrorGet() == I2C_ERROR_NONE)
 *        {
 *            // Write operation is successful
 *        }
 *        else
 *        {
 *            // Error handling
 *        }
 *    }
 *
 *    writeLength = 2; // 2 bytes of location address
 *    readLength = 1; // 1 byte read
 *    if (i2c1_host_interface.WriteRead(eepromAddr, transmitData, writeLength, readData , readLength))
 *    {
 *        while ( i2c1_host_interface.IsBusy())
 *        {
 *            i2c1_host_interface.Tasks();
 *        }
 *
 *        if (  i2c1_host_interface.ErrorGet() == I2C_ERROR_NONE)
 *        {
 *            // WriteRead operation is successful
 *        }
 *        else
 *        {
 *            // Error handling
 *        }
 *    }
 *
 *    // Case 2:
 *    transmitData[0] = 0x00;  // load MSB of EEPROM location
 *    transmitData[1] = 0x20;  // load LSB of EEPROM location
 *    transmitData[2] = 0x55;  // load data
 *    writeLength = 3; // 2 bytes of location address + 1 byte data
 *    if (i2c1_host_interface.Write(eepromAddr, transmitData, writeLength))
 *    {
 *        while (i2c1_host_interface.IsBusy())
 *        {
 *            i2c1_host_interface.Tasks();
 *        }
 *
 *        if ( i2c1_host_interface.ErrorGet() == I2C_ERROR_NONE)
 *        {
 *            // Write operation is successful
 *        }
 *        else
 *        {
 *            // Error handling
 *        }
 *    }
 *
 *    writeLength = 2; // 2 bytes of location address
 *    if (i2c1_host_interface.Write(eepromAddr, transmitData, writeLength))
 *    {
 *        while (i2c1_host_interface.IsBusy())
 *        {
 *            i2c1_host_interface.Tasks();
 *        }
 *
 *        if ( i2c1_host_interface.ErrorGet() == I2C_ERROR_NONE)
 *        {
 *            // Write operation is successful
 *        }
 *        else
 *        {
 *            // Error handling
 *        }
 *    }
 *
 *    readLength = 1; // 1 byte read
 *    if (i2c1_host_interface.Read(eepromAddr, readData, readLength))
 *    {
 *        while (i2c1_host_interface.IsBusy())
 *        {
 *            i2c1_host_interface.Tasks();
 *        }
 *
 *        if ( i2c1_host_interface.ErrorGet() == I2C_ERROR_NONE)
 *        {
 *            // Read operation is successful
 *        }
 *        else
 *        {
 *            // Error handling
 *        }
 *    }
 *
 *     while (1)
 *     {
 *     }
 * }
 * @endcode
*/


#endif // end of I2C_HOST_INTERFACE_H