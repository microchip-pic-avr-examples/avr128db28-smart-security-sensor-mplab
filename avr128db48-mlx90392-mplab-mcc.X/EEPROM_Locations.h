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

//MLX90632 Temp Sensor Fields
    
#define MEM_DEVICE_ID1 (0 + EEPROM_START_ADDR)
#define MEM_DEVICE_ID2 (2 + MEM_DEVICE_ID1)
#define MEM_DEVICE_ID3 (2 + MEM_DEVICE_ID2)

#define MEM_P_R (2 + MEM_DEVICE_ID3)
#define MEM_P_G (4 + MEM_P_R)
#define MEM_P_T (4 + MEM_P_G)
#define MEM_P_O (4 + MEM_P_T)
#define MEM_EA  (4 + MEM_P_O)
#define MEM_EB  (4 + MEM_EA)
#define MEM_FA  (4 + MEM_EB)
#define MEM_FB  (4 + MEM_FA)
#define MEM_GA  (4 + MEM_FB)
#define MEM_GB  (4 + MEM_GA)
#define MEM_KA  (4 + MEM_GB)
#define MEM_HA  (4 + MEM_KA)
#define MEM_HB  (4 + MEM_HA)
    
//Memory Validation (2 bytes) - XOR of 16-bit Device IDs
#define MEM_VALIDATE        (4 + MEM_HB)

//Update Rate for Temp Sensor
#define TEMP_UPDATE_PERIOD  (2 + MEM_VALIDATE)
    
//Begin MLX90392 Fields
    
//Device ID of the Magnetometer
#define EEPROM_MLX90392_ID         (2 + TEMP_UPDATE_PERIOD)
    
//"Closed Window" Constants
#define CLOSED_THRESHOLD_X_MAX (1 + EEPROM_MLX90392_ID)    
#define CLOSED_THRESHOLD_X_MIN (2 + CLOSED_THRESHOLD_X_MAX)    
#define CLOSED_THRESHOLD_Y_MAX (2 + CLOSED_THRESHOLD_X_MIN)    
#define CLOSED_THRESHOLD_Y_MIN (2 + CLOSED_THRESHOLD_Y_MAX)    
#define CLOSED_THRESHOLD_Z_MAX (2 + CLOSED_THRESHOLD_Y_MIN)    
#define CLOSED_THRESHOLD_Z_MIN (2 + CLOSED_THRESHOLD_Z_MAX)    

//"Cracked Window" Constants
#define CRACKED_THRESHOLD_X_MAX (2 + CLOSED_THRESHOLD_Z_MIN)    
#define CRACKED_THRESHOLD_X_MIN (2 + CRACKED_THRESHOLD_X_MAX)    
#define CRACKED_THRESHOLD_Y_MAX (2 + CRACKED_THRESHOLD_X_MIN)    
#define CRACKED_THRESHOLD_Y_MIN (2 + CRACKED_THRESHOLD_Y_MAX)    
#define CRACKED_THRESHOLD_Z_MAX (2 + CRACKED_THRESHOLD_Y_MIN)    
#define CRACKED_THRESHOLD_Z_MIN (2 + CRACKED_THRESHOLD_Z_MAX)    
    
#ifdef	__cplusplus
}
#endif

#endif	/* MLX90632_EEPROM_DEFINES_H */

