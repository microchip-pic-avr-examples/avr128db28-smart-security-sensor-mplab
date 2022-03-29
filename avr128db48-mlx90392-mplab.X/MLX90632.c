#include "MLX90632.h"
#include "MLX90632_Defines.h"
#include "EEPROM_Locations.h"

#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "MLX90392.h"

#include "TWI0_host.h"

//I2C Address to Communicate at. Can be changed later
static uint8_t _MLX90632_address = MLX90632_I2C_ADDR_BASE;
static bool EEPROM_valid = false;

//Calibration Constants
static float_hex P_R, P_G, P_T, P_O, Ea, Eb, Fa, Fb, Ga, Gb, Ka, Ha, Hb;

//Measurements stored in the sensor's RAM, Device ID 
static int16_t RAM_results[6];

//Positions of memory (in terms from the DS)
#define RAM_4 RAM_results[0]
#define RAM_5 RAM_results[1]
#define RAM_6 RAM_results[2]
#define RAM_7 RAM_results[3]
#define RAM_8 RAM_results[4]
#define RAM_9 RAM_results[5]

#define CREATE_16BIT(MSB, LSB) ((int16_t)(((uint16_t) MSB << 8) | LSB))

//Convert 4 uint8_t to uint32_t
#define CREATE_32BIT(B4, B3, B2, B1) ((int32_t)(((uint32_t) B4 << 24) | ((uint32_t) B3 << 16) | ((uint16_t) B2 << 8) | (uint32_t) B1))

static uint16_t deviceID[3];

static volatile uint8_t cyclePos = 0; //Position of valid data
static uint8_t i2cBuffer[4];

static float ToDUT = 25.0, TaDUT = 25.0;

bool _readWriteMLX90632(MLX90632_Register reg, uint8_t* memory, uint8_t size)
{
    i2cBuffer[0] = (reg & 0xFF00) >> 8;
    i2cBuffer[1] = (reg & 0xFF);
    
    //Queue I2C
    bool success = TWI0_sendsAndReadBytes(_MLX90632_address, &i2cBuffer[0], 2, &memory[0], size);
    
    if (!success)
        return false;
    
    return true;

}

//Saves internal test constants to EEPROM memory
void _saveConstantsToEEPROM(void)
{
    //Save Device ID
    eeprom_write_word((uint16_t*) MEM_DEVICE_ID1, deviceID[0]);
    eeprom_write_word((uint16_t*) MEM_DEVICE_ID2, deviceID[1]);
    eeprom_write_word((uint16_t*) MEM_DEVICE_ID3, deviceID[2]);
        
    //Store Constants
    eeprom_write_dword((uint32_t*) MEM_P_R, P_R.hexCode);
    
    eeprom_write_dword((uint32_t*) MEM_P_G, P_G.hexCode);
    eeprom_write_dword((uint32_t*) MEM_P_T, P_T.hexCode);
    eeprom_write_dword((uint32_t*) MEM_P_O, P_O.hexCode);
    
    eeprom_write_dword((uint32_t*) MEM_EA, Ea.hexCode);
    eeprom_write_dword((uint32_t*) MEM_EB, Eb.hexCode);
    
    eeprom_write_dword((uint32_t*) MEM_FA, Fa.hexCode);
    eeprom_write_dword((uint32_t*) MEM_FB, Fb.hexCode);
    
    eeprom_write_dword((uint32_t*) MEM_GA, Ga.hexCode);
    eeprom_write_dword((uint32_t*) MEM_GB, Gb.hexCode);
    
    eeprom_write_dword((uint32_t*) MEM_KA, Ka.hexCode);
    
    eeprom_write_dword((uint32_t*) MEM_HA, Ha.hexCode);
    eeprom_write_dword((uint32_t*) MEM_HB, Hb.hexCode);
    
    //Store end-of-block marker
    eeprom_write_word((uint16_t*) MEM_VALIDATE, deviceID[0] ^ deviceID[1] ^ deviceID[2]);
}


//Initializes the sensor and the internal constants for calculations
bool MLX90632_initDevice(bool bypass)
{
    if (bypass || (!_MLX90632_loadConstantsFromEEPROM()))
    {
        if (!_MLX90632_loadConstantsFromDevice())
        {
            //Failed to load constants from device and EEPROM
#ifndef TEST_MLX90632
            return false;
#else
            _MLX90632_loadTestConstants();
#endif
        }
        
        //Saves constants to EEPROM
        //Currently doesn't handle write failures
        _saveConstantsToEEPROM();
    }
    else
    {
        EEPROM_valid = true;
    }
    
    return true;
}

//Attempts to load pre-computed sensor constants from internal EEPROM. 
//If CACHE_CONSTANTS is not defined, this function fails. 
bool _MLX90632_loadConstantsFromEEPROM(void)
{
#ifndef CACHE_CONSTANTS
    return false;
#else
   uint16_t EEPROM_id[3];
    uint16_t XORcheck;
    
    //Get the Device ID. This will be matched against the EEPROM
    if (!MLX90632_getDeviceID(&deviceID[0]))
        return false;
    
    //Next, get the device ID from MCU EEPROM    
    EEPROM_id[0] = eeprom_read_word((uint16_t*) MEM_DEVICE_ID1);
    EEPROM_id[1] = eeprom_read_word((uint16_t*) MEM_DEVICE_ID2);
    EEPROM_id[2] = eeprom_read_word((uint16_t*) MEM_DEVICE_ID3);
    
    //Compute XOR of device ID
    XORcheck = EEPROM_id[0] ^ EEPROM_id[1] ^ EEPROM_id[2];
    
    volatile uint16_t mValidate = eeprom_read_word((uint16_t*) MEM_VALIDATE);
    
    //Verify end-of-block memory marker
    if (XORcheck != mValidate)
    {
        return false;
    }
    
    //Verify Device ID
    if ((EEPROM_id[0] != deviceID[0]) || (EEPROM_id[1] != deviceID[1])
            || (EEPROM_id[2] != deviceID[2]))
    {
        return false;
    }
        
    //Load Constants from MCU EEPROM
    
    P_R.hexCode = (eeprom_read_dword((uint32_t*) MEM_P_R));
    P_G.hexCode = (eeprom_read_dword((uint32_t*) MEM_P_G)); 
    P_T.hexCode = (eeprom_read_dword((uint32_t*) MEM_P_T)); 
    P_O.hexCode = (eeprom_read_dword((uint32_t*) MEM_P_O)); 
    
    Ea.hexCode = (eeprom_read_dword((uint32_t*) MEM_EA)); 
    Eb.hexCode = (eeprom_read_dword((uint32_t*) MEM_EB)); 
    
    Fa.hexCode = (eeprom_read_dword((uint32_t*) MEM_FA)); 
    Fb.hexCode = (eeprom_read_dword((uint32_t*) MEM_FB)); 
    
    Ga.hexCode = (eeprom_read_dword((uint32_t*) MEM_GA)); 
    Gb.hexCode = (eeprom_read_dword((uint32_t*) MEM_GB)); 
    
    Ka.hexCode = (eeprom_read_dword((uint32_t*) MEM_KA)); 
    
    Ha.hexCode = (eeprom_read_dword((uint32_t*) MEM_HA)); 
    Hb.hexCode = (eeprom_read_dword((uint32_t*) MEM_HB)); 
    
    return true;
#endif

}

//Forces constants to be loaded from the sensor. 
//If CACHE_CONSTANTS is defined, constants are also stored in EEPROM.
bool _MLX90632_loadConstantsFromDevice(void)
{
    //Load the device ID
    if (!MLX90632_getDeviceID(&deviceID[0]))
        return false;
    
    //Pool of memory for reading constants
    uint8_t dataPool[24];
    
    //Queue I2C Operation
    bool success = _readWriteMLX90632(EE_P_R_LOW, &dataPool[0], 16);
    
    if (!success)
        return false;

    //Decode Constants
    P_R.value = CREATE_32BIT(dataPool[2], dataPool[3], dataPool[0], dataPool[1]) * P_R_MULT;
    P_G.value = CREATE_32BIT(dataPool[6], dataPool[7], dataPool[4], dataPool[5]) * P_G_MULT;
    P_T.value = CREATE_32BIT(dataPool[10], dataPool[11], dataPool[8], dataPool[9]) * P_T_MULT;
    P_O.value = CREATE_32BIT(dataPool[14], dataPool[15], dataPool[12], dataPool[13]) * P_O_MULT;
    
    //Skip to EE_EA
    
    success = _readWriteMLX90632(EE_EA_LOW, &dataPool[0], 24);
    
    if (!success)
        return false;
        
    //Create Constants
    
    Ea.value = CREATE_32BIT(dataPool[2], dataPool[3], dataPool[0], dataPool[1]) * EA_MULT;
    Eb.value = CREATE_32BIT(dataPool[6], dataPool[7], dataPool[4], dataPool[5]) * EB_MULT;
    
    Fa.value = CREATE_32BIT(dataPool[10], dataPool[11], dataPool[8], dataPool[9]) * FA_MULT;
    Fb.value = CREATE_32BIT(dataPool[14], dataPool[15], dataPool[12], dataPool[13]) * FB_MULT;
    
    Ga.value = CREATE_32BIT(dataPool[18], dataPool[19], dataPool[16], dataPool[17]) * GA_MULT;
    Gb.value = CREATE_16BIT(dataPool[20], dataPool[21]) * GB_MULT;
    
    Ka.value  = CREATE_16BIT(dataPool[22], dataPool[23]) * KA_MULT;
    
    //Skip to Ha   
    success = _readWriteMLX90632(EE_HA_LOW, &dataPool[0], 4);
    
    if (!success)
        return false;
    
    Ha.value  = CREATE_16BIT(dataPool[0], dataPool[1]) * HA_MULT;
    Hb.value  = CREATE_16BIT(dataPool[2], dataPool[3]) * HB_MULT;
    
    return true;
}

//Loads EXAMPLE values from datasheet.
void _MLX90632_loadTestConstants(void)
{   
    //Clear ID
    deviceID[0] = 0x00;
    deviceID[1] = 0x00;
    deviceID[2] = 0x00;
    
    //32-bit Constants
    P_R.value = (int32_t) 0x005D0103 * P_R_MULT;  
    P_G.value = (int32_t) 0x051CFAE5 * P_G_MULT;
    P_T.value = (int32_t) 0x00000000 * P_T_MULT;
    P_O.value = (int32_t) 0x00001900 * P_O_MULT; 
    
    Ea.value = (int32_t) 0x0051CFAE * EA_MULT;
    Eb.value = (int32_t) 0x005D0103 * EB_MULT;
    
    Fa.value = (int32_t) 0x03506351 * FA_MULT;
    Fb.value = (int32_t) 0xFE2571F1 * FB_MULT;
    
    Ga.value = (int32_t) 0xFDFFA7A4 * GA_MULT;
    
    //16-bit Constants
    Gb.value = (int16_t) 0x2600 * GB_MULT;
    
    Ka.value = (int16_t) 0x2A00 * KA_MULT;
    
    Ha.value = (int16_t) 0x4000 * HA_MULT;
    Hb.value = (int16_t) 0x0000 * HB_MULT;
}

//Loads EXAMPLE sensor data from datasheet.
void _MLX90632_loadTestData(void)
{
    //Load data
    RAM_results[0] = 0xFF9B;
    RAM_results[1] = 0xFF9D;
    RAM_results[2] = 0x57E4;
    RAM_results[3] = 0xFF97;
    RAM_results[4] = 0xFF99;
    RAM_results[5] = 0x59D8;
    
    //Set cycle position
    if (cyclePos == 1)
    {
        cyclePos = 2;
    }
    else
    {
        cyclePos = 1;
    }
}

//Returns true if constants were loaded from EEPROM
bool MLX90632_cacheOK(void)
{
    return EEPROM_valid;
}

//Returns the 48-bit device ID. ID must be at least 3 16-bit numbers or greater
bool MLX90632_getDeviceID(uint16_t* id)
{  
    uint8_t readBuffer[6];
    
    //Get ID
    bool success = _readWriteMLX90632(MLX90632_ID0, &readBuffer[0], 6);
    
    if (!success)
        return false;
    
    id[0] = CREATE_16BIT(readBuffer[0], readBuffer[1]);
    id[1] = CREATE_16BIT(readBuffer[2], readBuffer[3]);
    id[2] = CREATE_16BIT(readBuffer[4], readBuffer[5]);
    
    return true;
}

//Returns the status of the sensor
bool MLX90632_getStatus(MLX90632_Status* status)
{    
    return MLX90632_getRegister(MLX90632_REG_STATUS, &status->regValue);
}

//Retrieves a 16-bit value from a register
bool MLX90632_getRegister(MLX90632_Register reg, uint16_t* result)
{
    i2cBuffer[0] = (reg & 0xFF00) >> 8;
    i2cBuffer[1] = (reg & 0xFF);
    
    bool success = TWI0_sendsAndReadBytes(MLX90632_I2C_ADDR_BASE, &i2cBuffer[0], 2, &i2cBuffer[2] , 2);
    
    if (!success)
        return false;
        
    //Create 16-bit result
    *result = CREATE_16BIT(i2cBuffer[2], i2cBuffer[3]);
    
    return true;
}

//Refresh cached measurements and cycle position indicators
bool MLX90632_getResults(void)
{
#ifdef TEST_MLX90632
    //Load Test Pattern
    _MLX90632_loadTestData();
    return true;
#endif

    uint8_t statusBytes[2];
    
    bool success = _readWriteMLX90632(MLX90632_REG_STATUS, &statusBytes[0], 2);
    
    if (!success)
        return false;
    
    cyclePos = (statusBytes[1] & MLX90632_STATUS_CYCLE_POSITION_gm) >> MLX90632_STATUS_CYCLE_POSITION_gp;
    
    //Read Values from RAM
    
    uint8_t dataBuffer[12];
    
    success = _readWriteMLX90632(MLX90632_RAM4_START, &dataBuffer[0], 12);
    
    if (!success)
        return false;

    RAM_4 = CREATE_16BIT(dataBuffer[0], dataBuffer[1]);
    RAM_5 = CREATE_16BIT(dataBuffer[2], dataBuffer[3]);
    RAM_6 = CREATE_16BIT(dataBuffer[4], dataBuffer[5]);
    RAM_7 = CREATE_16BIT(dataBuffer[6], dataBuffer[7]);
    RAM_8 = CREATE_16BIT(dataBuffer[8], dataBuffer[9]);
    RAM_9 = CREATE_16BIT(dataBuffer[10], dataBuffer[11]);
    
    return true;
}

bool MLX90632_isDataReady(void)
{
    MLX90632_Status status;
    bool success;
    
    success = MLX90632_getStatus(&status);
    
    //If in test mode - data test pattern can be used as a fallback
    if (!success)
    {
#ifndef TEST_MLX90632
        return false;
#else
        return true;
#endif

    }
    
    return (status.regValue & MLX90632_STATUS_NEW_DATA_bm);
}

bool MLX90632_setRegister(MLX90632_Register reg, uint16_t data)
{
    i2cBuffer[0] = (reg & 0xFF00) >> 8;
    i2cBuffer[1] = (reg & 0xFF);
    i2cBuffer[2] = (data & 0xFF00) >> 8;
    i2cBuffer[3] = (data & 0xFF);
    
    //Queue I2C
    bool success = TWI0_sendBytes(_MLX90632_address, &i2cBuffer[0], 4);
    
    if (!success)
        return false;
        
    return true;
}

//Starts a single conversion
bool MLX90632_startSingleConversion(void)
{
    bool success = MLX90632_setRegister(MLX90632_REG_CONTROL, (MLX90632_CONTROL_MEAS_SELECT_MEDICAL << MLX90632_CONTROL_MEAS_SELECT_gp) | 
            MLX90632_CONTROL_SOB_bm | 
            (MLX90632_CONTROL_MODE_SLEEPING_STEP << MLX90632_CONTROL_MODE_gp));
    
    //If in test mode, then use hard-coded patterns if sensor fails
#ifdef TEST_MLX90632
    if (!success)
    {
        return true;
    }
#endif

    return success;
}

//Computes the temperature of an object. Returns the result in Celsius
bool MLX90632_computeTemperature(void)
{        
    //Cycle Position Invalid
    if ((cyclePos != 2) && (cyclePos != 1))
        return false;
    
    //Measurements are loaded - calculate results
    //From Section 11.1.1.1 in the DS
    
    float VRta = RAM_9 + Gb.value * (RAM_6 / 12.0F);
    float AMB = (RAM_6 / 12.0F) / VRta * EXP_2_P19;
    float S;
    
    if (cyclePos == 1)
    {
        S = (RAM_4 + RAM_5) / 2.0F;
    }
    else
    {
        S = (RAM_7 + RAM_8) / 2.0F;
    }
    
    float VRto = RAM_9 + Ka.value * (RAM_6 / 12.0F);
    float Sto = (S / 12.0F) / VRto * EXP_2_P19;
    
    TaDUT = P_O.value + ((AMB - P_R.value) / P_G.value) + P_T.value * ((AMB - P_R.value) * (AMB - P_R.value));
    float TaK = TaDUT + 273.15; //Ta in Kelvin
    
    ToDUT = 25.0;
    float _temp;
    
    for (uint8_t i = 0; i < CALCULATION_LOOPS; ++i)
    {
        _temp = Fa.value * Ha.value * (1 + Ga.value * (ToDUT - 25.0) + Fb.value * (TaDUT - 25.0));
        
        //Apply correction factor (if enabled)
#ifdef EMISSIVITY_ENABLED
        _temp *= EMISSIVITY_FACTOR;
#endif
        
        //Everything under the quartic root is calculated
        _temp = (Sto / _temp) + (TaK * TaK * TaK * TaK);
        
        //Calculate temperature
        ToDUT = sqrtf(sqrtf(_temp)) - 273.15 - Hb.value;
    }
    
    return true;
}

//Returns the Temperature of the Sensor
float MLX90632_getSensorTemp(void)
{
    return TaDUT;
}

//Returns the Temperature of the Object
float MLX90632_getObjectTemp(void)
{
    return ToDUT;
}