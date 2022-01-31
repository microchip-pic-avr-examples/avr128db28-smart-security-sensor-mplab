#ifndef MLX90632_H
#define	MLX90632_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
    
#include "MLX90632_Defines.h"

//Define CACHE_CONSTANTS to use EEPROM for storing sensor calibration values
//Reduces the startup time
#define CACHE_CONSTANTS
    
//Define TEST_MLX90632 to use test patterns if sensor communication fails.
//#define TEST_MLX90632
    
//Define EMISSIVITY_ENABLED to enable an emissivity factor.
//#define EMISSIVITY_ENABLED
#define EMISSIVITY_FACTOR 1.0
    
//Number of loops to run the iterative temperature calculation
#define CALCULATION_LOOPS 3
    
    //Initializes the sensor and the internal constants for calculations
    bool MLX90632_initDevice(bool bypass);
    
    //Attempts to load pre-computed sensor constants from internal EEPROM. 
    //If CACHE_CONSTANTS is not defined, this function fails. 
    bool _MLX90632_loadConstantsFromEEPROM(void);
    
    //Forces constants to be loaded from the sensor. 
    //If CACHE_CONSTANTS is defined, constants are also stored in EEPROM.
    bool _MLX90632_loadConstantsFromDevice(void);
    
    //Loads EXAMPLE values from datasheet.
    void _MLX90632_loadTestConstants(void);
    
    //Loads EXAMPLE sensor data from datasheet.
    void _MLX90632_loadTestData(void);
    
    //Returns true if the MCU's EEPROM cache was loaded
    bool MLX90632_cacheOK(void);
    
    //Returns the 48-bit device ID. ID must be at least 3 16-bit numbers or greater
    bool MLX90632_getDeviceID(uint16_t* id);
    
    //Retrieves a 16-bit value from a register
    bool MLX90632_getRegister(MLX90632_Register reg, uint16_t* result);

    //Returns the status of the sensor
    bool MLX90632_getStatus(MLX90632_Status* status);
    
    //Refresh cached measurements and cycle position indicators
    bool MLX90632_getResults(void);
    
    //Returns true if data is ready, and false if not ready (or if an error occurs)
    bool MLX90632_isDataReady(void);
    
    //Set a register on the sensor
    bool MLX90632_setRegister(MLX90632_Register reg, uint16_t data);
    
    //Starts a single conversion
    bool MLX90632_startSingleConversion(void);
    
    //Computes the temperature of an object. Returns the result in Celsius
    bool MLX90632_computeTemperature(void);
    
    //Returns the Temperature of the Sensor (in Celsius)
    float MLX90632_getSensorTemp(void);
    
    //Returns the Temperature of the Object (in Celsius)
    float MLX90632_getObjectTemp(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* MLX90632_H */

