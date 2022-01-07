/**
 *
 * @file nvm.c
 *
 * @ingroup nvm_driver
 *
 * @brief This file contains the implementation for NVM driver
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

#include <avr/pgmspace.h>
#include "../nvm.h"

static void (*EEPROM_Callback)(void);
static void EEPROM_DefaultCallback(void);

void NVM_Initialize(void)
{
    //APPCODEWP disabled; APPDATAWP disabled; BOOTRP disabled; FLMAP SECTION0; FLMAPLOCK disabled; 
    NVMCTRL.CTRLB = 0x0;

    EEPROM_CallbackRegister(EEPROM_DefaultCallback);

    //EEREADY disabled; 
    NVMCTRL.INTCTRL = 0x0;
}

bool EEPROM_IsBusy(void)
{
    return (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);
}

eeprom_data_t EEPROM_Read(eeprom_adr_t address)
{
    // Read operation will be stalled by hardware if any write is in progress
    return *(eeprom_data_t *) address;
}

void EEPROM_Write(eeprom_adr_t address, eeprom_data_t data)
{
    // Program the EEPROM with desired value(s)
    ccp_write_spm((void *) &NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);

    // Write byte to EEPROM
    *(uint8_t *) address = data;

    // Clear the current command
    ccp_write_spm((void *) &NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
}

void EEPROM_Task(void)
{
    if(NVMCTRL.INTFLAGS)
    {
        // The interrupt flag has to be cleared manually
        NVMCTRL.INTFLAGS |= NVMCTRL_EEREADY_bm;

        if (EEPROM_Callback != NULL)
        {
            EEPROM_Callback();
        }
    }
}

static void EEPROM_DefaultCallback(void)
{
    //Add your interrupt code here or
    //Use EEPROM_CallbackRegister() function to use Custom ISR
}

void EEPROM_CallbackRegister(void (* CallbackHandler)(void))
{
    EEPROM_Callback = CallbackHandler;
}

