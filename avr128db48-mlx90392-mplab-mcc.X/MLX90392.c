#include "MLX90392.h"
#include "mcc_generated_files/i2c_host/twi0.h"
#include "mcc_generated_files/timer/delay.h"
#include "mcc_generated_files/mvio/mvio.h"

#include <stdint.h>
#include <stdbool.h>

#define MLX90392_ADDR 0x0C
#define PROCESS_I2C() do { while ((I2C0_IsBusy()) && (MVIO_isOK())) { I2C0_Tasks(); }} while (0)

//Init the Sensor
bool MLX90392_init(void)
{
    bool success;
    
    //Reset device
    success = MLX90392_reset();
    
    if (!success)
        return false;
    
    //Setup Temperature Correction + Z-Axis Filtering
    success = MLX90392_setRegister(MLX90392_CUST_CTRL, MLX90392_CUST_CTRL_DNC | MLX90392_CUST_CTRL_T_COMP_EN_bm |
            (5 << MLX90392_CUST_CTRL_DIG_FILT_HALL_Z_gp));

    if (!success)
        return false;
    
    //Setup Digital Filtering for XY + Temp Measurements
    success = MLX90392_setRegister(MLX90392_OSC_DIG_FILT, (5 << MLX90392_OSR_DIG_FILT_OSR_HALL_XY_gp) | (5 << MLX90392_OSR_DIG_FILT_OSR_TEMP_MP_gp));
    
    return success;
}

//Returns the register specified from the device
bool MLX90392_getRegister(MLX90392_Register reg, uint8_t* data)
{
    //If MVIO is not powered, exit
    if (!MVIO_isOK())
        return false;
    
    //Start Read / Write Operation
    if (I2C0_WriteRead(MLX90392_ADDR, &reg, 1, data, 1))
    {
        PROCESS_I2C();

        //If an error occurred...
        if (I2C0_ErrorGet())
            return false;

        return true;
    }
        
    return false;

}

//Sets the register specified on the device
bool MLX90392_setRegister(MLX90392_Register reg, uint8_t data)
{
    if (!MVIO_isOK())
        return false;

    uint8_t writeBuffer[] = {reg, data};
    
    if (I2C0_Write(MLX90392_ADDR, &writeBuffer[0], 2))
    {
        PROCESS_I2C();

        //If an error occurred...
        if (I2C0_ErrorGet())
            return false;

        return true;
    }
        
    return false;
}

bool MLX90392_reset(void)
{
    return MLX90392_setRegister(MLX90392_RST, 0x06);
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

bool MLX90392_getSingleMeasurementBlocking(MLX90392_RawResult* result)
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
    MLX90392_RawResult result;
    
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
    return MLX90392_setRegister(MLX90392_CTRL, mode);
}

bool MLX90392_getResult(MLX90392_RawResult* result)
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