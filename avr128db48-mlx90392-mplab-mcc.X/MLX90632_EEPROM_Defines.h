#ifndef MLX90632_EEPROM_DEFINES_H
#define	MLX90632_EEPROM_DEFINES_H

#ifdef	__cplusplus
extern "C" {
#endif

#define EEPROM_START_ADDR 0x1400
    
/*
 * Important! These locations are for the MCU's EEPROM ONLY
 * Not for use with the sensor itself
 */

#define MEM_DEVICE_ID1 (0x00 + EEPROM_START_ADDR)
#define MEM_DEVICE_ID2 (0x02 + EEPROM_START_ADDR)
#define MEM_DEVICE_ID3 (0x04 + EEPROM_START_ADDR)

#define MEM_P_R (0x06 + EEPROM_START_ADDR)
#define MEM_P_G (0x0A + EEPROM_START_ADDR)
#define MEM_P_T (0x0E + EEPROM_START_ADDR)
#define MEM_P_O (0x12 + EEPROM_START_ADDR)
#define MEM_EA  (0x16 + EEPROM_START_ADDR)
#define MEM_EB  (0x1A + EEPROM_START_ADDR)
#define MEM_FA  (0x1E + EEPROM_START_ADDR)
#define MEM_FB  (0x22 + EEPROM_START_ADDR)
#define MEM_GA  (0x26 + EEPROM_START_ADDR)
#define MEM_GB  (0x2A + EEPROM_START_ADDR)
#define MEM_KA  (0x2E + EEPROM_START_ADDR)
#define MEM_HA  (0x32 + EEPROM_START_ADDR)
#define MEM_HB  (0x36 + EEPROM_START_ADDR)
    
//Memory Validation (2 bytes) - XOR of 16-bit Device IDs
#define MEM_VALIDATE (0x3A + EEPROM_START_ADDR)

    
#ifdef	__cplusplus
}
#endif

#endif	/* MLX90632_EEPROM_DEFINES_H */

