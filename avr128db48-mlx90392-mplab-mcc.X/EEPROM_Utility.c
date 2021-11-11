#include "EEPROM_Utility.h"
#include "mcc_generated_files/system/system.h"

#include <xc.h>
#include <stdint.h>

//Saves a 16-bit number to EEPROM, starting at addr
void save16BToEEPROM(uint16_t addr, uint16_t data)
{
    EEPROM_Write(addr, ((data & 0xFF00) >> 8));
    EEPROM_Write(addr + 1, (data & 0xFF));
}

//Saves a 32-bit number to EEPROM, starting at addr
void save32BToEEPROM(uint16_t addr, uint32_t data)
{
    EEPROM_Write(addr, ((data & 0xFF000000) >> 24));
    EEPROM_Write(addr + 1, ((data & 0xFF0000) >> 16));
    EEPROM_Write(addr + 2, ((data & 0xFF00) >> 8));
    EEPROM_Write(addr + 3, (data & 0xFF));
}

int32_t get32BFromEEPROM(uint16_t address)
{
    int32_t value = 0x00000000;
    
    uint8_t counter = 4;
    
    while (counter != 0)
    {
        //Shift Value 1 byte
        value <<= 8;
        
        //Load Byte
        value |= EEPROM_Read(address);
        
        counter--;
        address++;
    }
    
    return value;
}

int16_t get16BFromEEPROM(uint16_t address)
{
    int16_t value = 0x0000;
    
    //Load Byte
    value |= EEPROM_Read(address);
    
    //Shift Value 1 byte
    value <<= 8;

    //Load Byte
    value |= EEPROM_Read(address + 1);
    
    return value;
}
