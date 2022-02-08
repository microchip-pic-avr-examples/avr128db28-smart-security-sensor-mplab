#ifndef MLX90632_EEPROM_DEFINES_H
#define	MLX90632_EEPROM_DEFINES_H

#ifdef	__cplusplus
extern "C" {
#endif
    
/*
 * Important! These locations are for the MCU's EEPROM ONLY
 * Not for use with the sensor itself
 */

//MLX90632 Temp Sensor Fields
    
#define MEM_DEVICE_ID1 (0x00)                   //0x1400
#define MEM_DEVICE_ID2 (2 + MEM_DEVICE_ID1)     //0x1402
#define MEM_DEVICE_ID3 (2 + MEM_DEVICE_ID2)     //0x1404

#define MEM_P_R (2 + MEM_DEVICE_ID3)            //0x1406
#define MEM_P_G (4 + MEM_P_R)                   //0x140A
#define MEM_P_T (4 + MEM_P_G)                   //0x140E
#define MEM_P_O (4 + MEM_P_T)                   //0x1412
#define MEM_EA  (4 + MEM_P_O)                   //0x1416
#define MEM_EB  (4 + MEM_EA)                    //0x141A
#define MEM_FA  (4 + MEM_EB)                    //0x141E
#define MEM_FB  (4 + MEM_FA)                    //0x1422
#define MEM_GA  (4 + MEM_FB)                    //0x1426
#define MEM_GB  (4 + MEM_GA)                    //0x142A
#define MEM_KA  (4 + MEM_GB)                    //0x142E
#define MEM_HA  (4 + MEM_KA)                    //0x1432
#define MEM_HB  (4 + MEM_HA)                    //0x1436
    
//Memory Validation (2 bytes) - XOR of 16-bit Device IDs
#define MEM_VALIDATE (4 + MEM_HB)               //0x143A

//Update Rate for Temp Sensor
#define TEMP_UPDATE_PERIOD (2 + MEM_VALIDATE)   //0x143C
    
//High Temperature Warning
#define TEMP_WARNING_HIGH_LOCATION (2 + TEMP_UPDATE_PERIOD)  //0x143E
    
//High Temperature Warning
#define TEMP_WARNING_LOW_LOCATION (4 + TEMP_WARNING_HIGH_LOCATION)    //0x1443
    
//Temperature Units
#define TEMP_UNIT_LOCATION (4 + TEMP_WARNING_LOW_LOCATION)

//Begin MLX90392 Fields
    
//Device ID of the Magnetometer
#define EEPROM_MLX90392_ID (1 + TEMP_UNIT_LOCATION)     //0x1444
    
//Cracked Window Threshold
#define CRACKED_THRESHOLD_V (1 + EEPROM_MLX90392_ID)    //0x1445
#define MAX_VALUE_V (4 + CRACKED_THRESHOLD_V)           //0x1449
    
//Self-Calibration Offset
#define MAGNETOMETER_OFFSET_X (4 + MAX_VALUE_V)     //0x144C   
#define MAGNETOMETER_OFFSET_Y (2 + MAGNETOMETER_OFFSET_X)   //0x144F    
#define MAGNETOMETER_OFFSET_Z (2 + MAGNETOMETER_OFFSET_Y)   //0x1451
    
//Self-Calibration Normalizers (if negative, invert value, if positive)
#define MAGNETOMETER_SCALER_X (2 + MAGNETOMETER_OFFSET_Z)   //0x144D
#define MAGNETOMETER_SCALER_Y (1 + MAGNETOMETER_SCALER_X)   //0x144E
#define MAGNETOMETER_SCALER_Z (1 + MAGNETOMETER_SCALER_Y)   //0x144F

//If angles are not used, EEPROM ends at 0x1450
    
//Angle Ranges
#define MAGNETOMETER_MIN_XY (1 + MAGNETOMETER_SCALER_Z)     //0x1450
#define MAGNETOMETER_MAX_XY (2 + MAGNETOMETER_MIN_XY)       //0x1451
#define MAGNETOMETER_MIN_XZ (2 + MAGNETOMETER_MAX_XY)       //0x1452
#define MAGNETOMETER_MAX_XZ (2 + MAGNETOMETER_MIN_XZ)       //0x1453
#define MAGNETOMETER_MIN_YZ (2 + MAGNETOMETER_MAX_XZ)       //0x1454
#define MAGNETOMETER_MAX_YZ (2 + MAGNETOMETER_MIN_YZ)       //0x1455
    
//End of EEPROM (with angles) - 0x1456
    
#ifdef	__cplusplus
}
#endif

#endif	/* MLX90632_EEPROM_DEFINES_H */

