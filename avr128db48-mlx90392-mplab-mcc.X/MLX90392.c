#include "MLX90392.h"
#include "mcc_generated_files/i2c_host/twi0.h"
#include "mcc_generated_files/timer/delay.h"
#include "mcc_generated_files/mvio/mvio.h"

#include <stdint.h>
#include <stdbool.h>

#define MLX90392_ADDR 0x0C
#define PROCESS_I2C() do { while (I2C0_IsBusy()) { I2C0_Tasks(); }} while (0)


bool MLX90392_reset(void)
{
    //If MVIO is not ready, then exit.
    if (!MVIO_isOK())
        return false;
    
    //Reset MLX90392 Device
    uint8_t initSEQ[2] = {(uint8_t) RST, 0x06};
    
    if (I2C0_Write(MLX90392_ADDR, &initSEQ[0], 2))
    {
        PROCESS_I2C();
        
        if (I2C0_ErrorGet())
            return false;
        
        return true;
    }
    
    return false;
}

bool MLX90392_isDataReady(void)
{
    //If MVIO is not ready, then exit.
    if (!MVIO_isOK())
        return false;
    
    uint8_t status = 0x00;
    
    //I2C Error
    if (!I2C0_Read(MLX90392_ADDR, &status, 1))
        return false;
    
    PROCESS_I2C();
    
    if (I2C0_ErrorGet())
        return false;
    
    //DRDY is bit 0 in status
    return (status & 0x01);
}

bool MLX90392_getSingleMeasurement(MLX90392_Result* result)
{
    //Set the Sensor into Single Measurement Mode
    if (!MLX90392_setOperatingMode(SINGLE))
        return false;
    
    do
    {
        //Wait...
        DELAY_milliseconds(1);
        
    } while (!MLX90392_isDataReady());
    
    //Get Result
    return MLX90392_getResult(result);
}

bool MLX90392_selfTest(void)
{
    //First, put device into idle
    if (!MLX90392_setOperatingMode(IDLE))
        return false;
    
    //Then switch to self-test
    if (!MLX90392_setOperatingMode(SELF_TEST))
        return false;
    
    do
    {
        //Wait...
        DELAY_milliseconds(1);
        
    } while (!MLX90392_isDataReady());
    
    //Read measurement data
    MLX90392_Result result;
    
    //Get results
    if (!MLX90392_getResult(&result))
        return false;
    
    //X-Axis Check
    if (!((MLX90392_X_LOW_PASS <= result.X_Meas) && 
            (result.X_Meas <= MLX90392_X_HIGH_PASS)))
    {
        asm("NOP");
        return false;
    }
        
    
    //Y-Axis Check
    if (!((MLX90392_Y_LOW_PASS <= result.Y_Meas) &&
            (result.Y_Meas <= MLX90392_Y_HIGH_PASS)))
    {
        asm("NOP");
        return false;
    }
    
    //Z-Axis Check
    if (!((MLX90392_Z_LOW_PASS <= result.Z_Meas) &&
            (result.Z_Meas <= MLX90392_Z_HIGH_PASS)))
    {
        asm("NOP");
        return false;
    }
    
    //Everything passes the tests
    return true;
}



bool MLX90392_setOperatingMode(MLX90392_Mode mode)
{
    //If MVIO is not ready, then exit.
    if (!MVIO_isOK())
        return false;
    
    uint8_t setMode[] = { CTRL, mode};
    
    //Set the operating mode
    if (I2C0_Write(MLX90392_ADDR, &setMode[0], 2))
    {
        PROCESS_I2C();
        
        //If an error occurred...
        if (I2C0_ErrorGet())
            return false;
        
        return true;
    }
        
    return false;
}

bool MLX90392_getResult(MLX90392_Result* result)
{    
    //If MVIO is not ready, then exit.
    if (!MVIO_isOK())
        return false;
    
    //Read from 0x00 to 0x07    
    if (I2C0_Read(MLX90392_ADDR, &result->data[0], 8))
    {
        PROCESS_I2C();
        
        //If an error occurred...
        if (I2C0_ErrorGet())
            return false;
        
        return true;
    }

    return false;
}