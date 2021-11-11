#ifndef EEPROM_UTILITY_H
#define	EEPROM_UTILITY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
    
//Convert 2 uint8_t to uint16_t
#define CREATE_16BIT(MSB, LSB) ((int16_t)(((uint16_t) MSB << 8) | LSB))

//Convert 4 uint8_t to uint32_t
#define CREATE_32BIT(B4, B3, B2, B1) ((int32_t)(((uint32_t) B4 << 24) | ((uint32_t) B3 << 16) | ((uint16_t) B2 << 8) | (uint32_t) B1))

    //Saves a 16-bit number to EEPROM, starting at addr
void save16BToEEPROM(uint16_t addr, uint16_t data);

//Saves a 32-bit number to EEPROM, starting at addr
void save32BToEEPROM(uint16_t addr, uint32_t data);

//Get a 32-bit number from EEPROM
int32_t get32BFromEEPROM(uint16_t address);

//Get a 16-bit number from EEPROM
int16_t get16BFromEEPROM(uint16_t address);


#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_UTILITY_H */

