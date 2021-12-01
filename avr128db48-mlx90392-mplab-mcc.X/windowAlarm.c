#include "windowAlarm.h"
#include "mcc_generated_files/system/system.h"
#include "MLX90392.h"
#include "EEPROM_Locations.h"
#include "EEPROM_Utility.h"
#include "mcc_generated_files/timer/delay.h"

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#include "printUtility.h"

//Enumeration for the Measurement State Machine
typedef enum {
MAGNETOMETER_START = 0, MAGNETOMETER_WAIT, MAGNETOMETER_ERROR, 
        MAGNETOMETER_REINIT_WAIT, MAGNETOMETER_REINIT
} MagentometerMeasState;

//Enumeration for Calibration State Machine
typedef enum{
    CAL_BAD = 0, CAL_CLOSED_WAIT, CAL_CLOSED, CAL_CRACKED_WAIT, CAL_CRACKED, CAL_OPEN_WAIT, CAL_OPEN, CAL_GOOD
} MagnetometerCalibrationState;

//Enumeration for the Result Printing State Machine
typedef enum{
    WINDOW_CLOSED = 0, WINDOW_CRACKED, WINDOW_OPEN, INVALID
} MagnetometerResultState;

//Calibration Status
static MagnetometerCalibrationState calState = CAL_BAD;
static MagnetometerResultState prevResultState = INVALID;

//State Machine Variables
static volatile MagentometerMeasState magState = MAGNETOMETER_START;
static volatile uint16_t magCounter = MAGNETOMETER_ERROR_DELAY;

static uint32_t crackedV;
static int16_t offsetX = 0, offsetY = 0, offsetZ = 0;
static int8_t scaleX = 1, scaleY = 1, scaleZ = 1;
static int8_t minXY, maxXY, minXZ, maxXZ, minYZ, maxYZ;


//Thresholds
//static volatile int16_t X_min_closed, X_max_closed, Y_min_closed, Y_max_closed, 
//                Z_min_closed, Z_max_closed;
//
//static volatile int16_t X_min_cracked, X_max_cracked, Y_min_cracked, Y_max_cracked, 
//                Z_min_cracked, Z_max_cracked;

static uint8_t sensorID = 0x00;

//Internal Function to compare the measurement and print the status
void _windowAlarm_processResult(MLX90392_RawResult* result)
{
    
}

int8_t _windowAlarm_computeScalingFactor(int16_t result)
{
    int8_t scale = 1;
    bool isNegative = false;
    
    //Invert result - apply correction at the end
    if (result < 0)
    {
        result *= -1;
        isNegative = true;
    }
 
    //While result doesn't fit in an 7-bit number
    while (result > 127)
    {
        result = result >> 1;
        scale++;
    }
    
    //Correct scale if negative
    if (isNegative)
        scale *= -1;
    
    //Max Scaling Value to convert 16-bit to 8-bit
    return scale;
}

//Internal function for setting the trigger thresholds (calibration)
void _windowAlarm_runCalibration(MLX90392_RawResult* result)
{
    static uint16_t sampleCount = 0;
    
    //Averages for zeroing
    static int32_t averageX, averageY, averageZ, averageR2;

    MLX90392_NormalizedResults normResults;
    windowAlarm_createNormalizedResults(result, &normResults);
    
    //Vector Result
    uint32_t vectorResult = ((uint32_t)result->X_Meas * result->X_Meas) +
                            ((uint32_t)result->Y_Meas * result->Y_Meas) +
                            ((uint32_t)result->Z_Meas * result->Z_Meas);
    
    switch (calState)
    {
        case CAL_BAD:
        {
            printConstantString("Open the window and press the button to start.\r\n");
            calState = CAL_OPEN_WAIT;
            
            //Clear Averaging Variables
            averageX = 0;
            averageY = 0;
            averageZ = 0;
            break;
        }
        case CAL_OPEN_WAIT:
        {
            if (SW0_GetValue())
            {
                printConstantString("Beginning Open Window Calibration.\r\n");
                calState = CAL_OPEN;
                sampleCount = 0;
            }
            break;
        }
        case CAL_OPEN:
        {
            sampleCount++;
            
            //Accumulate Results
            averageX += result->X_Meas;
            averageY += result->Y_Meas;
            averageZ += result->Z_Meas;
            
            if (sampleCount >= MAGNETOMETER_CALIBRATION_SAMPLES)
            {
                //Compute Average Offset
                offsetX = round(averageX / MAGNETOMETER_CALIBRATION_SAMPLES);
                offsetY = round(averageY / MAGNETOMETER_CALIBRATION_SAMPLES);
                offsetZ = round(averageZ / MAGNETOMETER_CALIBRATION_SAMPLES);
                
                sprintf(getCharBuffer(), "Zeroing Complete. X_off = %d, Y_off = %d, Z_off=%d\r\n", offsetX, offsetY, offsetZ);
                printBufferedString();
                printConstantString("Please close the window and the press the button to continue.\r\n");
                calState = CAL_CLOSED_WAIT;
            }
            break;
        }
        case CAL_CLOSED_WAIT:
        {
            if (SW0_GetValue())
            {
                printConstantString("Beginning Closed Window Calibration.\r\n");
                calState = CAL_CLOSED;
                sampleCount = 0;
            }
            break;
        }
        case CAL_CLOSED:
        {                        
            sampleCount++;
            
            //Accumulate results
            averageX += (result->X_Meas - offsetX);
            averageY += (result->Y_Meas - offsetY);
            averageZ += (result->Z_Meas - offsetZ);
            
            if (sampleCount >= MAGNETOMETER_CALIBRATION_SAMPLES)
            {
                //Compute Average
                averageX = round(averageX / MAGNETOMETER_CALIBRATION_SAMPLES);
                averageY = round(averageY / MAGNETOMETER_CALIBRATION_SAMPLES);
                averageZ = round(averageZ / MAGNETOMETER_CALIBRATION_SAMPLES);
                
                //Compute Scaling Factor
                scaleX = _windowAlarm_computeScalingFactor(averageX);
                scaleY = _windowAlarm_computeScalingFactor(averageY);
                scaleZ = _windowAlarm_computeScalingFactor(averageZ);
                
                sprintf(getCharBuffer(), "Scaling Completed. X_scale  = 2^%d, Y_scale = 2^%d, Z_scale = 2^%d\r\n",
                        scaleX, scaleY, scaleZ);
                printBufferedString();
                printConstantString("Please crack the window to set the alarm threshold, then press the button.\r\n");
                calState = CAL_CRACKED_WAIT;
                
                //Compute Normalized
                windowAlarm_createNormalizedResults(result, &normResults);
                
                //Init Angles
                
                minXY = normResults.xyAngle;
                maxXY = normResults.xyAngle;
                
                minXZ = normResults.xzAngle;
                maxXZ = normResults.xzAngle;
                
                minYZ = normResults.yzAngle;
                maxYZ = normResults.yzAngle;
            }
            break;
        }
        case CAL_CRACKED_WAIT:
        {            
            if (minXY > normResults.xyAngle)
            {
                minXY = normResults.xyAngle;
            }
            else if (normResults.xyAngle > maxXY)
            {
                maxXY = normResults.xyAngle;
            }
            
            if (minXZ > normResults.xyAngle)
            {
                minXZ = normResults.xyAngle;
            }
            else if (normResults.xyAngle > maxXZ)
            {
                maxXZ = normResults.xyAngle;
            }
            
            if (minYZ > normResults.xyAngle)
            {
                minYZ = normResults.xyAngle;
            }
            else if (normResults.xyAngle > maxYZ)
            {
                maxYZ = normResults.xyAngle;
            }
            
            if (SW0_GetValue())
            {
                sprintf(getCharBuffer(), "Allowed Angles: %d < XY < %d, %d < XZ < %d, %d < YZ < %d\r\n",
                        minXY, maxXY, minXZ, maxXZ, minYZ, maxYZ);
                printBufferedString();
                
                printConstantString("Beginning Cracked Window Calibration.\r\n");
                calState = CAL_CRACKED;
                sampleCount = 0;
                averageR2 = 0;
            }
            break;
        }
        case CAL_CRACKED:
        {         
            sampleCount++;
            
            averageR2 += normResults.r2;
            
            if (sampleCount >= MAGNETOMETER_CALIBRATION_SAMPLES)
            {
                //Set Cracked Threshold
                crackedV = round(averageR2 / MAGNETOMETER_CALIBRATION_SAMPLES);
                
                sprintf(getCharBuffer(), "Calibration Completed. Threshold: %lu\r\nCrack the window and press the button to continue\r\n", crackedV);
                printBufferedString();
                
                calState = CAL_GOOD;
            }
            break;
        }
        case CAL_GOOD:
        {
            calState = CAL_BAD;
            break;
        }
    }
}

void windowAlarm_init(bool safeStart)
{
    bool success;
    
    //Print Welcome
    printConstantString("Initializing MLX90392 Magnetometer Sensor...");
    
    success = MLX90392_init();

    //If unable to init...
    if (!success)
    {
        printConstantString("FAILED\r\n");
        magState = MAGNETOMETER_ERROR;
        return;
    }
    
    uint8_t _verify_sensorID;
    success = MLX90392_getRegister(MLX90392_DEVICE_ID, &_verify_sensorID);
    
    //If unable to get ID
    if (!success)
    {
        printConstantString("FAILED\r\n");
        magState = MAGNETOMETER_ERROR;
        return;
    }
    
    uint8_t EEPROM_id_test = EEPROM_Read(EEPROM_MLX90392_ID);
    
    if ((!safeStart) && (EEPROM_id_test == _verify_sensorID))
    {       
        //EEPROM is valid, load thresholds
        /*X_max_closed = EEPROM_Read(CLOSED_THRESHOLD_X_MAX);
        X_min_closed = EEPROM_Read(CLOSED_THRESHOLD_X_MIN);
        
        Y_max_closed = EEPROM_Read(CLOSED_THRESHOLD_Y_MAX);
        Y_min_closed = EEPROM_Read(CLOSED_THRESHOLD_Y_MIN);
        
        Z_max_closed = EEPROM_Read(CLOSED_THRESHOLD_Z_MAX);
        Z_min_closed = EEPROM_Read(CLOSED_THRESHOLD_Z_MIN);
        
        X_max_cracked = EEPROM_Read(CRACKED_THRESHOLD_X_MAX);
        X_min_cracked = EEPROM_Read(CRACKED_THRESHOLD_X_MIN);
        
        Y_max_cracked = EEPROM_Read(CRACKED_THRESHOLD_Y_MAX);
        Y_min_cracked = EEPROM_Read(CRACKED_THRESHOLD_Y_MIN);
        
        Z_max_cracked = EEPROM_Read(CRACKED_THRESHOLD_Z_MAX);
        Z_min_cracked = EEPROM_Read(CRACKED_THRESHOLD_Z_MIN);*/
        
        //calState = CAL_GOOD;
    }
    else
    {
        //EEPROM is not valid
        calState = CAL_BAD;
    }
    
    printConstantString("OK\r\n");
}

//Converts raw results into a normalized compressed value
void windowAlarm_createNormalizedResults(MLX90392_RawResult* raw, MLX90392_NormalizedResults* results)
{
    if (scaleX < 0)
    {
        //Inverted Result
        results->x = -1 * ((raw->X_Meas - offsetX) >> (scaleX * -1));
    }
    else
    {
        //Non-Inverted Result
        results->x = (raw->X_Meas - offsetX) >> (scaleX);
    }
    
    if (scaleY < 0)
    {
        //Inverted Result
        results->y = -1 * ((raw->Y_Meas - offsetY) >> (scaleY * -1));
    }
    else
    {
        //Non-Inverted Result
        results->y = (raw->Y_Meas - offsetY) >> (scaleY);
    }
    
    if (scaleZ < 0)
    {
        //Inverted Result
        results->z = -1 * ((raw->Z_Meas - offsetZ) >> (scaleZ * -1));
    }
    else
    {
        //Non-Inverted Result
        results->z = (raw->Z_Meas - offsetZ) >> (scaleZ);
    }
    
    results->r2 = (uint16_t)(results->x * results->x) + 
            (uint16_t)(results->y * results->y) + 
            (uint16_t)(results->z * results->z);
    
    if (MAGNETOMETER_ANGLE_THRESHOLD <= results->r2)
    {
        if (results->y != 0)
            results->xyAngle = round((results->x * MAGNETOMETER_ANGLE_SCALE) / results->y);
        else
            results->xyAngle = 0;

        if (results->z != 0)
        {
            results->xzAngle = round((results->x * MAGNETOMETER_ANGLE_SCALE) / results->z);
            results->yzAngle = round((results->y * MAGNETOMETER_ANGLE_SCALE) / results->z);
        }
        else
        {
            results->xzAngle = 0;
            results->yzAngle = 0;
        }
    }
    else
    {
        results->xyAngle = 0;
        results->xzAngle = 0;
        results->yzAngle = 0;
    }
}

void compareVector(MLX90392_RawResult* result)
{
    MLX90392_NormalizedResults normResults;
    windowAlarm_createNormalizedResults(result, &normResults);
    bool alarmOK = false;

    if (normResults.r2 >= crackedV)
    {
        //Within Expected Intensity
        
        
        /*sprintf(getCharBuffer(), "XY = %d, XZ = %d, YZ = %d\r\n", 
                normResults.xyAngle, normResults.xzAngle, normResults.yzAngle);
        printBufferedString();*/
        
        //Check Angles
        if ((normResults.xyAngle > maxXY) || (normResults.xyAngle < minXY))
        {
            alarmOK = false;
#ifdef MAGNETOMETER_DEBUG_PRINT
            printConstantString("<XY Angle Error> - ");
#endif 
        }
        else if ((normResults.xzAngle > maxXZ) || (normResults.xzAngle < minXZ))
        {
            alarmOK = false;
#ifdef MAGNETOMETER_DEBUG_PRINT
            printConstantString("<XZ Angle Error> - ");
#endif 
        }
        else if ((normResults.xzAngle > maxXZ) || (normResults.xzAngle < minXZ))
        {
            alarmOK = false;
#ifdef MAGNETOMETER_DEBUG_PRINT
            printConstantString("<YZ Angle Error> - ");
#endif 
        }
        else
        {
            alarmOK = true;
        }

    }
    else
    {
#ifdef MAGNETOMETER_DEBUG_PRINT
        printConstantString("<Threshold Exceeded> - ");
#endif 
    }
    
    if (alarmOK)
    {
        printConstantString("Alarm Good\r\n");
    }
    else
    {
        printConstantString("Alarm Bad\r\n");
    }
}

//Saves current thresholds
bool windowAlarm_saveThresholds(void)
{
    //Remove this line when ready
    return false;
    
    if (!MLX90392_getRegister(MLX90392_DEVICE_ID, &sensorID))
        return false;
        
    /*save16BToEEPROM(CLOSED_THRESHOLD_X_MAX, (uint16_t) X_max_closed);
    save16BToEEPROM(CLOSED_THRESHOLD_X_MIN, (uint16_t) X_min_closed);
    save16BToEEPROM(CLOSED_THRESHOLD_Y_MAX, (uint16_t) Y_max_closed);
    save16BToEEPROM(CLOSED_THRESHOLD_Y_MIN, (uint16_t) Y_min_closed);
    save16BToEEPROM(CLOSED_THRESHOLD_Z_MAX, (uint16_t) Z_max_closed);
    save16BToEEPROM(CLOSED_THRESHOLD_Z_MIN, (uint16_t) Z_min_closed);
    
    save16BToEEPROM(CRACKED_THRESHOLD_X_MAX, (uint16_t) X_max_cracked);
    save16BToEEPROM(CRACKED_THRESHOLD_X_MIN, (uint16_t) X_min_cracked);
    save16BToEEPROM(CRACKED_THRESHOLD_Y_MAX, (uint16_t) Y_max_cracked);
    save16BToEEPROM(CRACKED_THRESHOLD_Y_MIN, (uint16_t) Y_min_cracked);
    save16BToEEPROM(CRACKED_THRESHOLD_Z_MAX, (uint16_t) Z_max_cracked);
    save16BToEEPROM(CRACKED_THRESHOLD_Z_MIN, (uint16_t) Z_min_cracked);*/
    
    //Write the ID of the sensor
    //EEPROM_Write(EEPROM_MLX90392_ID, sensorID);

    return true;
}

void windowAlarm_FSM(void)
{    
    bool success;
    
    //MLX90392 Magnetometer State Machine
    switch (magState)
    {
        case MAGNETOMETER_START:
        {                        
            //Start a single conversion
            success = MLX90392_setOperatingMode(SINGLE);

            //Move to next state
            if (success)
            {
                magState = MAGNETOMETER_WAIT;
            }
            else
            {
                //Something went wrong
                magState = MAGNETOMETER_ERROR;
            }

            break;
        }
        case MAGNETOMETER_WAIT:
        {
            success = MLX90392_isDataReady();

            //If data is ready
            if (success)
            {
                MLX90392_RawResult magResult;
                success = MLX90392_getResult(&magResult);
                if (success)
                {
                    //If set, print the values of the magnetometer
#ifdef MAGNETOMETER_PRINT_CSV
                    sprintf(getCharBuffer(), "%d, %d, %d\r\n", magResult.X_Meas, magResult.Y_Meas, magResult.Z_Meas);
                    printBufferedString();
#elif MAGNETOMETER_RAW_VALUE_PRINT 
                    uint32_t v_result =  ((uint32_t) magResult.X_Meas * magResult.X_Meas) + 
                             ((uint32_t) magResult.Y_Meas * magResult.Y_Meas)
                            + ((uint32_t) magResult.Z_Meas * magResult.Z_Meas);
                    float xy_angle = (float) magResult.X_Meas / magResult.Y_Meas; //atan(magResult.X_Meas / (float) magResult.Y_Meas);
                    float yz_angle = (float) magResult.Y_Meas / magResult.Z_Meas; //atan(magResult.Y_Meas / (float) magResult.Z_Meas);
                    float xz_angle = (float) magResult.X_Meas / magResult.Z_Meas;
                    sprintf(getCharBuffer(), "X: %d, Y: %d, Z: %d\r\nVector: %lu\r\nAngle XY: %f\r\nAngle YZ: %f\r\nAngle XZ: %f\r\n",
                        magResult.X_Meas, magResult.Y_Meas, magResult.Z_Meas, v_result, xy_angle, yz_angle, xz_angle);
                printBufferedString();
#endif
                    if ((calState == CAL_GOOD) && (!SW0_GetValue()))
                    {
                        //Process Result if Cal is valid
                        //_windowAlarm_processResult(&magResult);
                        compareVector(&magResult);
                    }
                    else
                    {
                        //Calibration Active
                        _windowAlarm_runCalibration(&magResult);
                        
                    }
                    magState = MAGNETOMETER_START;
                }
                else
                {
                    //Something went wrong
                    magState = MAGNETOMETER_ERROR;
                }
            }
            break;
        }
        case MAGNETOMETER_REINIT_WAIT:
        {
            magState = MAGNETOMETER_REINIT;
            break;
        }
        case MAGNETOMETER_REINIT:
        {
            success = MLX90392_init();
            if (success)
            {
                prevResultState = INVALID;
                magState = MAGNETOMETER_START;
                magCounter = MAGNETOMETER_ERROR_DELAY;
            }
            else
            {
                magState = MAGNETOMETER_ERROR;
            }
            break;
        }
        case MAGNETOMETER_ERROR:
        default:
        {
            //Simple delay to keep this error from filling the UART.
            if (magCounter >= MAGNETOMETER_ERROR_DELAY)
            {
                printConstantString("Magnetometer Sensor Error - Reboot Device\r\n");
                magCounter = 1;
            }
            else
            {
                magCounter++;
            }

            break;
        }
    }
}

//Run this ISR if the MVIO changes readiness
void _windowAlarm_onMVIOChange(void)
{
    if (MVIO_isOK() && (magState == MAGNETOMETER_ERROR))
    {
        //Power Restored to MVIO
        magState = MAGNETOMETER_REINIT_WAIT;
    }
    else if (!MVIO_isOK())
    {
        //Loss of Power on MVIO
        magState = MAGNETOMETER_ERROR;
    }
}

void _windowAlarm_buttonPressed(void)
{
    LED0_Toggle();
}