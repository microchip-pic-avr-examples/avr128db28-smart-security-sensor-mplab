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

#ifndef EEPROM_LOCATIONS_H
#define	EEPROM_LOCATIONS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
/*
 * Important! These locations are for the MCU's EEPROM ONLY
 * Not for use with the sensor itself
 */

//Layout Version of the EEPROM
//If EEPROM is changed, update this value to force safemode on the first startup
#define EEPROM_LAYOUT_VERSION_ID 0x01
    
//Layout Version Address
#define MEM_EEPROM_LAYOUT_VERSION_ID 0x00
    
//MLX90632 Temp Sensor Fields
    
#define MEM_DEVICE_ID1 (1 + MEM_EEPROM_LAYOUT_VERSION_ID)
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
#define MEM_VALIDATE (4 + MEM_HB)
        
//High Temperature Warning
#define TEMP_WARNING_HIGH_LOCATION (2 + MEM_VALIDATE)
    
//High Temperature Warning
#define TEMP_WARNING_LOW_LOCATION (4 + TEMP_WARNING_HIGH_LOCATION)
    
//Temperature Units
#define TEMP_UNIT_LOCATION (4 + TEMP_WARNING_LOW_LOCATION)
    
//Sets whether the FIR sensor operates in sleep to monitor for abnormal temps.
#define TEMP_MONITOR_SLEEP_ENABLE (1 + TEMP_UNIT_LOCATION)

//Begin MLX90392 Fields
    
//Device ID of the Magnetometer
#define MEM_MLX90392_ID (1 + TEMP_MONITOR_SLEEP_ENABLE)
    
//Cracked Window Threshold
#define CRACKED_THRESHOLD_V (1 + MEM_MLX90392_ID)
#define MAX_VALUE_V (4 + CRACKED_THRESHOLD_V)
    
//Self-Calibration Offset
#define MAGNETOMETER_OFFSET_X (4 + MAX_VALUE_V)
#define MAGNETOMETER_OFFSET_Y (2 + MAGNETOMETER_OFFSET_X) 
#define MAGNETOMETER_OFFSET_Z (2 + MAGNETOMETER_OFFSET_Y)
    
//Self-Calibration Normalizers (if negative, invert value, if positive)
#define MAGNETOMETER_SCALER_X (2 + MAGNETOMETER_OFFSET_Z)
#define MAGNETOMETER_SCALER_Y (1 + MAGNETOMETER_SCALER_X)
#define MAGNETOMETER_SCALER_Z (1 + MAGNETOMETER_SCALER_Y)
    
//Max/Min X
#define MAGNETOMETER_MAX_X (1 + MAGNETOMETER_SCALER_Z)
#define MAGNETOMETER_MIN_X (1 + MAGNETOMETER_MAX_X)
    
//Max/Min Y
#define MAGNETOMETER_MAX_Y (1 + MAGNETOMETER_MIN_X)
#define MAGNETOMETER_MIN_Y (1 + MAGNETOMETER_MAX_Y)
    
//Max/Min Z
#define MAGNETOMETER_MAX_Z (1 + MAGNETOMETER_MIN_Y)
#define MAGNETOMETER_MIN_Z (1 + MAGNETOMETER_MAX_Z)
    
//Begin System Parameters
    
//If set to GOOD_VALUE, then settings are OK
#define SYSTEM_GOOD_MARKER (1 + MAGNETOMETER_MIN_Z)
    
//Update Rate for Full System
#define SYSTEM_UPDATE_PERIOD (1 + SYSTEM_GOOD_MARKER)

//How many RTC pulses the Bluetooth is allowed to be idle (and unconnected) for     
#define SYSTEM_BLUETOOTH_IDLE_PERIOD (2 + SYSTEM_UPDATE_PERIOD)
            
//Angle Ranges
#define MAGNETOMETER_MIN_XY (2 + SYSTEM_UPDATE_PERIOD)     //0x1450
#define MAGNETOMETER_MAX_XY (2 + MAGNETOMETER_MIN_XY)       //0x1451
#define MAGNETOMETER_MIN_XZ (2 + MAGNETOMETER_MAX_XY)       //0x1452
#define MAGNETOMETER_MAX_XZ (2 + MAGNETOMETER_MIN_XZ)       //0x1453
#define MAGNETOMETER_MIN_YZ (2 + MAGNETOMETER_MAX_XZ)       //0x1454
#define MAGNETOMETER_MAX_YZ (2 + MAGNETOMETER_MIN_YZ)       //0x1455
        
#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_LOCATIONS_H */

