#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#include "printUtility.h"
#include "windowAlarm.h"
#include "windowAlarm_config.h"
#include "MLX90392.h"
#include "EEPROM_Locations.h"
#include "EEPROM_Utility.h"


//Enumeration for the Measurement State Machine
typedef enum {
MAGNETOMETER_START = 0, MAGNETOMETER_WAIT, MAGNETOMETER_ERROR, 
        MAGNETOMETER_REINIT_WAIT, MAGNETOMETER_REINIT
} MagentometerMeasState;

//Enumeration for Calibration State Machine
typedef enum{
    CAL_BAD = 0, CAL_INIT, CAL_OPEN_WAIT, CAL_OPEN, CAL_CLOSED_WAIT, CAL_CLOSED, 
            CAL_CRACKED_WAIT, CAL_CRACKED_ERR, CAL_CRACKED, CAL_CLOSED_FINAL_WAIT, 
            CAL_DEINIT, CAL_GOOD
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

//Alarm Parameters
static uint32_t crackedV, maxV;
static int16_t offsetX = 0, offsetY = 0, offsetZ = 0;
static uint8_t scaleX = 1, scaleY = 1, scaleZ = 1;

static bool buttonPressed = false, lastButtonState = false;

//Angle Ranges
#ifdef MAGNETOMETER_ANGLE_CHECK
static int16_t minXY, maxXY, minXZ, maxXZ, minYZ, maxYZ;
#endif

//ID of Magnetometer
static uint8_t sensorID = 0x00;

//Alarm Count
static uint8_t alarmState = 0;

uint8_t _windowAlarm_computeScalingFactor(int16_t result)
{
    uint8_t scale = 1;
    
    //Negate Input
    if (result < 0)
    {
        result *= -1;
    }
 
    //While result doesn't fit in an 7-bit number
    while (result > 127)
    {
        result = result >> 1;
        scale++;
    }
    
    return scale;
}

void windowAlarm_updateAngleMaxMin(MLX90392_NormalizedResults* normResults)
{
#ifdef MAGNETOMETER_ANGLE_CHECK
    if (minXY > normResults->xyAngle)
    {
        minXY = normResults->xyAngle;
    }
    else if (normResults->xyAngle > maxXY)
    {
        maxXY = normResults->xyAngle;
    }

    if (minXZ > normResults->xyAngle)
    {
        minXZ = normResults->xyAngle;
    }
    else if (normResults->xyAngle > maxXZ)
    {
        maxXZ = normResults->xyAngle;
    }

    if (minYZ > normResults->xyAngle)
    {
        minYZ = normResults->xyAngle;
    }
    else if (normResults->xyAngle > maxYZ)
    {
        maxYZ = normResults->xyAngle;
    }
#endif
}

//Internal function for setting the trigger thresholds (calibration)
void windowAlarm_runCalibration(MLX90392_RawResult* rawResult, MLX90392_NormalizedResults* normResults)
{
    static uint16_t sampleCount = 0;
    static uint8_t blinkCount = 0;
    
    //Digital Counter to blink the LED
    if (blinkCount >= MAGNETOMETER_CAL_BLINK_PERIOD)
    {
        LED0_Toggle();
        blinkCount = 0;
    }
    else
    {
        blinkCount++;
    }
    
    //Update Angle Max and Min
#ifdef MAGNETOMETER_ANGLE_CHECK
    windowAlarm_updateAngleMaxMin(normResults);
#endif
    
    //Monitor field strength, record strongest magnitude
    if (normResults->r2 > maxV)
    {
        maxV = normResults->r2;
    }
    
    //Averages for zeroing
    static int32_t averageX, averageY, averageZ, averageR2;
    
    switch (calState)
    {
        
        case CAL_BAD:
        {
            printConstantStringUSB("Open the window and press the button to start.\r\n");
            calState = CAL_INIT;
            
            //Clear Averaging Variables
            averageX = 0;
            averageY = 0;
            averageZ = 0;
            break;
        }
        case CAL_INIT:
        {
            if (MLX90392_setOperatingMode(MAGNETOMETER_CAL_SAMPLE_RATE))
            {
                calState = CAL_OPEN_WAIT;
            }
            else
            {
                printConstantStringUSB("[ERR] Failed to change MLX90392 operating mode.\r\n");
            }
            break;
        }
        case CAL_OPEN_WAIT:
        {
            if (buttonPressed)
            {
                printConstantStringUSB("Beginning Open Window Calibration.\r\n");
                calState = CAL_OPEN;
                sampleCount = 0;
            }
            break;
        }
        case CAL_OPEN:
        {
            sampleCount++;
            
            //Accumulate Results
            averageX += rawResult->X_Meas;
            averageY += rawResult->Y_Meas;
            averageZ += rawResult->Z_Meas;
            
            if (sampleCount >= MAGNETOMETER_CALIBRATION_SAMPLES)
            {
                //Compute Average Offset
                offsetX = round(averageX / MAGNETOMETER_CALIBRATION_SAMPLES);
                offsetY = round(averageY / MAGNETOMETER_CALIBRATION_SAMPLES);
                offsetZ = round(averageZ / MAGNETOMETER_CALIBRATION_SAMPLES);
                
                sprintf(getCharBufferUSB(), "Zeroing Complete. X_off = %d, Y_off = %d, Z_off= %d\r\n", offsetX, offsetY, offsetZ);
                printBufferedStringUSB();
                printConstantStringUSB("Please close the window and the press the button to continue.\r\n");
                calState = CAL_CLOSED_WAIT;
            }
            break;
        }
        case CAL_CLOSED_WAIT:
        {
            if (buttonPressed)
            {
                //Reset maximum value
                maxV = 0;
                
                printConstantStringUSB("Beginning Closed Window Calibration.\r\n");
                calState = CAL_CLOSED;
                sampleCount = 0;
            }
            break;
        }
        case CAL_CLOSED:
        {                        
            sampleCount++;
            
            //Accumulate results
            averageX += (rawResult->X_Meas - offsetX);
            averageY += (rawResult->Y_Meas - offsetY);
            averageZ += (rawResult->Z_Meas - offsetZ);
            
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
                
                sprintf(getCharBufferUSB(), "Scaling Completed. X_scale  = 2^%d, Y_scale = 2^%d, Z_scale = 2^%d\r\n",
                        scaleX, scaleY, scaleZ);
                printBufferedStringUSB();
                printConstantStringUSB("Please crack the window to set the alarm threshold, then press the button.\r\n");
                calState = CAL_CRACKED_WAIT;
                
#ifdef MAGNETOMETER_ANGLE_CHECK
                //Compute Normalized Results Immediately from New Values
                windowAlarm_createNormalizedResults(rawResult, normResults);
                
                //Init Angles
                minXY = normResults->xyAngle;
                maxXY = normResults->xyAngle;
                
                minXZ = normResults->xzAngle;
                maxXZ = normResults->xzAngle;
                
                minYZ = normResults->yzAngle;
                maxYZ = normResults->yzAngle;
#endif
            }
            break;
        }
        case CAL_CRACKED_WAIT:
        {                        
            if (normResults->r2 <= MAGNETOMETER_NOISE_MARGIN)
            {
                printConstantStringUSB("[ERR] Sensor value is below noise margin, please close the window and press the button to retry.\r\n");
                calState = CAL_CRACKED_ERR;
            }
            else if (buttonPressed)
            {
                printConstantStringUSB("Beginning Cracked Window Calibration.\r\n");
                calState = CAL_CRACKED;
                sampleCount = 0;
                averageR2 = 0;
            }
            break;
        }
        case CAL_CRACKED_ERR:
        {
            if (buttonPressed)
            {
                printConstantStringUSB("Please crack the window to set the alarm threshold, then press the button.\r\n");
                
#ifdef MAGNETOMETER_ANGLE_CHECK
                //Init Angles
                minXY = normResults->xyAngle;
                maxXY = normResults->xyAngle;
                
                minXZ = normResults->xzAngle;
                maxXZ = normResults->xzAngle;
                
                minYZ = normResults->yzAngle;
                maxYZ = normResults->yzAngle;
#endif
                //Update State
                calState = CAL_CRACKED_WAIT;
            }
            break;
        }
        case CAL_CRACKED:
        {   
            //Increment Counter
            sampleCount++;
            
            //Accumulate Values
            averageR2 += normResults->r2;
            
            if (sampleCount >= MAGNETOMETER_CALIBRATION_SAMPLES)
            {
                //Set Cracked Threshold
                crackedV = round(averageR2 / MAGNETOMETER_CALIBRATION_SAMPLES);

                //Warning for Low Noise Margin
                if (crackedV < MAGNETOMETER_NOISE_MARGIN)
                {
                    printConstantStringUSB("[WARN] Threshold is below noise margin. Sensor may not operate correctly.\r\n");
                }
                
                //Apply Tolerance, if set
#ifdef MAGNETOMETER_VECTOR_TOLERANCE
                crackedV = round(crackedV * (1.0 + MAGNETOMETER_VECTOR_TOLERANCE));
                maxV = round(maxV * (1.0 + MAGNETOMETER_VECTOR_TOLERANCE));
#endif
                
                //Print Threshold
                sprintf(getCharBufferUSB(), "Threshold calibration complete. Threshold: %lu\r\n", crackedV);
                printBufferedStringUSB();
                
                printConstantStringUSB("Please close the window and press the button to finish.\r\n");
                                
                //Update Cal State
                calState = CAL_CLOSED_FINAL_WAIT;
            }
            break;
        }
        case CAL_CLOSED_FINAL_WAIT:
        {
            if (buttonPressed)
            {
#ifdef MAGNETOMETER_ANGLE_CHECK
                
                //If set, add tolerance to the angles
#ifdef MAGNETOMETER_ANGLE_TOLERANCE
                minXY -= MAGNETOMETER_ANGLE_TOLERANCE;
                maxXY += MAGNETOMETER_ANGLE_TOLERANCE;
                
                minXZ -= MAGNETOMETER_ANGLE_TOLERANCE;
                maxXZ += MAGNETOMETER_ANGLE_TOLERANCE;
                
                minYZ -= MAGNETOMETER_ANGLE_TOLERANCE;
                maxYZ += MAGNETOMETER_ANGLE_TOLERANCE;
#endif
                
                //Print Angle Range
                sprintf(getCharBufferUSB(), "Allowed Angles: %d < XY < %d, %d < XZ < %d, %d < YZ < %d\r\n",
                        minXY, maxXY, minXZ, maxXZ, minYZ, maxYZ);
                printBufferedStringUSB();
#endif
                
                printConstantStringUSB("Calibration Completed.\r\n");
                calState = CAL_DEINIT;
                
                //Clear the blink counter
                blinkCount = 0;
                
                //Save values to EEPROM
                windowAlarm_saveThresholds();

            }
            break;
        }
        case CAL_DEINIT:
        {
            if (MLX90392_setOperatingMode(MAGNETOMETER_ACTIVE_SAMPLE_RATE))
            {
                calState = CAL_GOOD;
            }
            else
            {
                printConstantStringUSB("[ERR] Failed to change MLX90392 operating mode.\r\n");
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
    printConstantStringUSB("Initializing MLX90392 Magnetometer Sensor...");
    
    //Init Sensor
    success = MLX90392_init();

    //If unable to init...
    if (!success)
    {
        printConstantStringUSB("FAILED\r\n");
        magState = MAGNETOMETER_ERROR;
        return;
    }
    
    //If unable to init EEPROM constants
    if (!windowAlarm_loadFromEEPROM(safeStart))
    {
        printConstantStringUSB("CAL BAD\r\n");
        calState = CAL_BAD;
        return;
    }
    
    printConstantStringUSB("OK\r\n");
}

//Tries to load constants from EEPROM - called by windowAlarm_init
//Returns true if successful, or false if EEPROM is invalid
bool windowAlarm_loadFromEEPROM(bool safeStart)
{
    //Set Calibration Status
    calState = CAL_BAD;
    
    uint8_t _verify_sensorID;
    bool success = MLX90392_getRegister(MLX90392_DEVICE_ID, &_verify_sensorID);
    
    if (!success)
        return false;
    
    uint8_t EEPROM_id_test = EEPROM_Read(EEPROM_MLX90392_ID);
    
    if ((!safeStart) && (EEPROM_id_test == _verify_sensorID))
    {       
        //EEPROM is valid, load thresholds
        
        //Window Vector Threshold
        crackedV = get32BFromEEPROM(CRACKED_THRESHOLD_V);
        
        //Max Allowed Vector Strength
        maxV = get32BFromEEPROM(MAX_VALUE_V);
        
        //Offsets
        offsetX = get16BFromEEPROM(MAGNETOMETER_OFFSET_X);
        offsetY = get16BFromEEPROM(MAGNETOMETER_OFFSET_Y);
        offsetZ = get16BFromEEPROM(MAGNETOMETER_OFFSET_Z);
        
        //Scaling Values
        scaleX = EEPROM_Read(MAGNETOMETER_SCALER_X);
        scaleY = EEPROM_Read(MAGNETOMETER_SCALER_Y);
        scaleZ = EEPROM_Read(MAGNETOMETER_SCALER_Z);
        
#ifdef MAGNETOMETER_ANGLE_CHECK
        
        //Angle Ranges
        minXY = get16BFromEEPROM(MAGNETOMETER_MIN_XY);
        maxXY = get16BFromEEPROM(MAGNETOMETER_MAX_XY);
        minXZ = get16BFromEEPROM(MAGNETOMETER_MIN_XZ);
        maxXZ = get16BFromEEPROM(MAGNETOMETER_MAX_XZ);
        minYZ = get16BFromEEPROM(MAGNETOMETER_MIN_YZ);
        maxYZ = get16BFromEEPROM(MAGNETOMETER_MAX_YZ);
        
#endif      
        calState = CAL_GOOD;
        
        return true;
    }
    return false;
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
    
#ifdef MAGNETOMETER_ANGLE_CHECK
    
    if (MAGNETOMETER_NOISE_MARGIN <= results->r2)
    {
        if (results->y != 0)
            results->xyAngle = round((/*abs(results->x) * */ results->x * MAGNETOMETER_ANGLE_SCALE) /
                    (/*abs(results->y) * */ results->y));
        else
            results->xyAngle = 0;

        if (results->z != 0)
        {
            results->xzAngle = round((/*abs(results->x) * */ results->x * MAGNETOMETER_ANGLE_SCALE) / 
                    (/*abs(results->z) * */ results->z));
            results->yzAngle = round((/*abs(results->y) * */ results->y * MAGNETOMETER_ANGLE_SCALE) / 
                    (/*abs(results->z) * */ results->z));
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
#else
    //Set values to 0
    results->xyAngle = 0;
    results->xzAngle = 0;
    results->yzAngle = 0;
#endif
    
}

void windowAlarm_processResults(MLX90392_NormalizedResults* normResults)
{
    static uint8_t counter = 0;
    bool alarmOK = false;

    if ((normResults->r2 >= crackedV) && (normResults->r2 <= maxV))
    {
        //Within Expected Intensity

#ifdef MAGNETOMETER_ANGLE_CHECK        
        //Check Angles
        if ((normResults->xyAngle > maxXY) || (normResults->xyAngle < minXY))
        {
            alarmOK = false;
#ifdef MAGNETOMETER_DEBUG_PRINT
            sprintf(getCharBufferUSB(), "XY Range: %d < XY < %d, found %d\r\n", minXY, maxXY, normResults->xyAngle);
            printBufferedStringUSB();
            printConstantStringUSB("<XY Angle Error> - ");
#endif 
        }
        else if ((normResults->xzAngle > maxXZ) || (normResults->xzAngle < minXZ))
        {
            alarmOK = false;
#ifdef MAGNETOMETER_DEBUG_PRINT
            sprintf(getCharBufferUSB(), "XZ Range: %d < XZ < %d, found %d\r\n", minXZ, maxXZ, normResults->xzAngle);
            printBufferedStringUSB();
            printConstantStringUSB("<XZ Angle Error> - ");
#endif 
        }
        else if ((normResults->xzAngle > maxXZ) || (normResults->xzAngle < minXZ))
        {
            alarmOK = false;
#ifdef MAGNETOMETER_DEBUG_PRINT
            sprintf(getCharBufferUSB(), "YZ Range: %d < YZ < %d, found %d\r\n", minYZ, maxYZ, normResults->yzAngle);
            printBufferedStringUSB();
            printConstantStringUSB("<YZ Angle Error> - ");
#endif 
        }
        else
        {
            alarmOK = true;
        }
#else
        alarmOK = true;
#endif

    }
    else
    {
#ifdef MAGNETOMETER_DEBUG_PRINT
        sprintf(getCharBufferUSB(), "Max Value: %lu, Cracked: %lu, found %lu\r\n", maxV, crackedV, normResults->r2);
        printBufferedStringUSB();
        printConstantStringUSB("<Threshold Exceeded> - ");
#endif 
    }
    
    if (alarmOK)
    {
        if (alarmState != 0)
        {
            alarmState--;
        }
        LED0_SetLow();
    }
    else
    {
        if (alarmState != MAGNETOMETER_ALARM_TRIGGER_MAX)
        {
            alarmState++;
        }
        LED0_SetHigh();
    }
        
    if ((counter == MAGNETOMETER_ALARM_PRINT_RATE) || (alarmState == MAGNETOMETER_ALARM_TRIGGER))
    {
        if (alarmState >= MAGNETOMETER_ALARM_TRIGGER)
        {
            printConstantStringUSB("Alarm BAD\r\n");
        }
        else
        {
            printConstantStringUSB("Alarm OK\r\n");
        }

        counter = 0;
    }
    else
    {
        counter++;
    }
 }

//Saves current calibration parameters
bool windowAlarm_saveThresholds(void)
{    
    if (!MLX90392_getRegister(MLX90392_DEVICE_ID, &sensorID))
        return false;
        
    save32BToEEPROM(CRACKED_THRESHOLD_V, crackedV);
    save32BToEEPROM(MAX_VALUE_V, maxV);
    
    save16BToEEPROM(MAGNETOMETER_OFFSET_X, offsetX);
    save16BToEEPROM(MAGNETOMETER_OFFSET_Y, offsetY);
    save16BToEEPROM(MAGNETOMETER_OFFSET_Z, offsetZ);
    
    EEPROM_Write(MAGNETOMETER_SCALER_X, scaleX);
    EEPROM_Write(MAGNETOMETER_SCALER_Y, scaleY);
    EEPROM_Write(MAGNETOMETER_SCALER_Z, scaleZ);
    
#ifdef MAGNETOMETER_ANGLE_CHECK
    
    save16BToEEPROM(MAGNETOMETER_MIN_XY, minXY);
    save16BToEEPROM(MAGNETOMETER_MAX_XY, maxXY);
    save16BToEEPROM(MAGNETOMETER_MIN_XZ, minXZ);
    save16BToEEPROM(MAGNETOMETER_MAX_XZ, maxXZ);
    save16BToEEPROM(MAGNETOMETER_MIN_YZ, minYZ);
    save16BToEEPROM(MAGNETOMETER_MAX_YZ, maxYZ);
    
#endif
    
    //Write the ID of the sensor
    EEPROM_Write(EEPROM_MLX90392_ID, sensorID);

    return true;
}

void windowAlarm_FSM(void)
{    
    //Software button debouncer
    //Button is cleared at the end of MAGNETOMETER_WAIT
    if ((!lastButtonState) && (SW0_GetValue()))
    {
        //Button is pressed
        lastButtonState = true;
        buttonPressed = true;
    }
    else if ((lastButtonState) && (!SW0_GetValue()))
    {
        //Button released (reset state machine)
        lastButtonState = false;
    }
    
    bool success;
    
    //MLX90392 Magnetometer State Machine
    switch (magState)
    {
        case MAGNETOMETER_START:
        {                        
            //Configure Conversion Rates
            if (calState == CAL_GOOD)
            {
                //Monitor Mode - Low Speed, Low Power
                success = MLX90392_setOperatingMode(MAGNETOMETER_ACTIVE_SAMPLE_RATE);
            }
            else
            {
                //Calibration Mode - Higher Speed Operation
                success = MLX90392_setOperatingMode(MAGNETOMETER_CAL_SAMPLE_RATE);
            }
            

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
                MLX90392_NormalizedResults normResult;
                success = MLX90392_getResult(&magResult);
                if (success)
                {
                    windowAlarm_createNormalizedResults(&magResult, &normResult);
                    
                    //If set, print the raw values of the magnetometer
#ifdef MAGNETOMETER_PRINT_CSV
                    sprintf(getCharBufferUSB(), "%d, %d, %d\r\n", magResult.X_Meas, magResult.Y_Meas, magResult.Z_Meas);
                    printBufferedStringUSB();
#elif MAGNETOMETER_RAW_VALUE_PRINT 
                    sprintf(getCharBufferUSB(), "<RAW Values>\r\nX: %d, Y: %d, Z: %d\r\n",
                        magResult.X_Meas, magResult.Y_Meas, magResult.Z_Meas); 
                    printBufferedStringUSB();
                    
                    sprintf(getCharBufferUSB(), "<Normalized Values>\r\nX: %d, Y: %d, Z: %d, R^2: %lu\r\n",
                        normResult.x, normResult.y, normResult.z, normResult.r2);
                    printBufferedStringUSB();
#endif
                    if ((calState == CAL_GOOD) && (!buttonPressed))
                    {
                        //Process Result if Cal is valid
                        windowAlarm_processResults(&normResult);
                    }
                    else
                    {
                        //Calibration Active
                        windowAlarm_runCalibration(&magResult, &normResult);
                        
                    }
                    //magState = MAGNETOMETER_START;
                }
                else
                {
                    //Something went wrong
                    magState = MAGNETOMETER_ERROR;
                }
                
                //Clear Button Press
                buttonPressed = false;
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
                
                //Attempt to load calibration constants from EEPROM
                windowAlarm_loadFromEEPROM(false);
                
                //Update state machine
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
                printConstantStringUSB("Magnetometer Sensor Error - Reboot Device\r\n");
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
        
        //Triggers immediate message print
        magCounter = MAGNETOMETER_ERROR_DELAY;
    }
}