#include "EEPROM_Utility.h"
#include "mcc_generated_files/system/system.h"

#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdint.h>

//Saves an 8-bit number to EEPROM, starting at addr    
void save8BToEEPROM(uint16_t addr, uint8_t data)
{
    // Program the EEPROM with desired value(s)
    ccp_write_spm((void *) &NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);

    // Write byte to EEPROM
    *(uint8_t *) addr = data;

    // Clear the current command
    ccp_write_spm((void *) &NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
}

//Saves a 16-bit number to EEPROM, starting at addr
void save16BToEEPROM(uint16_t addr, uint16_t data)
{
    save8BToEEPROM(addr, ((data & 0xFF00) >> 8));
    save8BToEEPROM(addr + 1, (data & 0xFF));
}

//Saves a 32-bit number to EEPROM, starting at addr
void save32BToEEPROM(uint16_t addr, uint32_t data)
{
    save8BToEEPROM(addr, ((data & 0xFF000000) >> 24));
    save8BToEEPROM(addr + 1, ((data & 0xFF0000) >> 16));
    save8BToEEPROM(addr + 2, ((data & 0xFF00) >> 8));
    save8BToEEPROM(addr + 3, (data & 0xFF));
}

uint32_t get32BFromEEPROM(uint16_t address)
{
    uint32_t value = 0x00000000;
    
    uint8_t counter = 4;
    
    while (counter != 0)
    {
        //Shift Value 1 byte
        value <<= 8;
        
        //Load Byte
        value |= get8BFromEEPROM(address);
        
        counter--;
        address++;
    }
    
    return value;
}

uint16_t get16BFromEEPROM(uint16_t address)
{
    uint16_t value = 0x0000;
    
    //Load Byte
    value |= get8BFromEEPROM(address);
    
    //Shift Value 1 byte
    value <<= 8;

    //Load Byte
    value |= get8BFromEEPROM(address + 1);
    
    return value;
}

uint8_t get8BFromEEPROM(uint16_t address)
{    
    return *(uint8_t*) address;
}