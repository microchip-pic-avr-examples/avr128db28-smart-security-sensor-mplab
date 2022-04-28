#include "GPIO.h"

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <avr/eeprom.h>

#include "printUtility.h"
#include "windowAlarm.h"
#include "windowAlarm_config.h"
#include "MLX90392.h"
#include "EEPROM_Locations.h"
#include "MVIO.h"
#include "windowAlarm_messages.h"
#include "RN4870.h"
#include "TWI0_host.h"
#include "LEDcontrol.h"
#include "Bluetooth_Timeout_Timer.h"

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
//static volatile uint16_t magCounter = MAGNETOMETER_ERROR_DELAY;

//Alarm Parameters
static uint32_t crackedV, maxV;
static int16_t offsetX = 0, offsetY = 0, offsetZ = 0;
static uint8_t scaleX = 1, scaleY = 1, scaleZ = 1;

//If set, the next data will be printed to the RN4870
static bool requestPrint = false;

//Angle Ranges
#ifdef MAGNETOMETER_ANGLE_CHECK
static int16_t minXY, maxXY, minXZ, maxXZ, minYZ, maxYZ;
#endif

//Result Print, Current Button State, Last Button State
static bool alarmResultsReady = false, buttonPressed = false, lastButtonState = false;

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

void windowAlarm_updateAngleMaxMin(MLX90392_NormalizedResults8* normResults)
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

//Starts a calibration sequence, but does NOT interrupt it.
void windowAlarm_requestCalibration(void)
{
    calState = CAL_BAD;
}

//Internal function for setting the trigger thresholds (calibration)
void windowAlarm_runCalibration(MLX90392_RawResult16* rawResult, MLX90392_NormalizedResults8* normResults)
{
    static MagnetometerCalibrationState oldState = CAL_BAD;
    static uint16_t sampleCount = 0;
    static uint8_t weightedAlarmState = 0;
    
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
            //Configure Instructions State Machine
            oldState = CAL_BAD;
                    
            //Advance State Machine
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
                alarmResultsReady = true;
            }
            else
            {
                USB_sendStringWithEndline("[ERR] Failed to change MLX90392 operating mode.");
            }
            break;
        }
        case CAL_OPEN_WAIT:
        {
            if (buttonPressed)
            {
                USB_sendStringWithEndline("Starting open window calibration.");
                RN4870_sendStringToUser("Beginning open window calibration.");
                calState = CAL_OPEN;
                sampleCount = 0;
            }
            break;
        }
        case CAL_OPEN:
        {
            if (windowAlarm_isMagneticOverflow(rawResult))
            {
                RN4870_sendStringToUser("[ERR] Magnetic Field is too strong. Please retry.");
                calState = CAL_OPEN_WAIT;
            }
            
            sampleCount++;
            
            //Accumulate Results
            averageX += rawResult->X_Meas;
            averageY += rawResult->Y_Meas;
            averageZ += rawResult->Z_Meas;
            
            if (sampleCount >= MAGNETOMETER_CALIBRATION_SAMPLES)
            {
                //Compute Average Offset
                offsetX = averageX >> MAGNETOMETER_RSHIFT_SAMPLE_COUNT;
                offsetY = averageY >> MAGNETOMETER_RSHIFT_SAMPLE_COUNT;
                offsetZ = averageZ >> MAGNETOMETER_RSHIFT_SAMPLE_COUNT;
                
                //Print Debug Info
                sprintf(USB_getCharBuffer(), "Zeroing Complete. X_off = %d, Y_off = %d, Z_off= %d\r\n", offsetX, offsetY, offsetZ);
                USB_sendBufferedString();
                
                //Update State
                calState = CAL_CLOSED_WAIT;            
            }
            break;
        }
        case CAL_CLOSED_WAIT:
        {
            if (buttonPressed)
            {             
                USB_sendStringWithEndline("Starting closed window calibration.");
                RN4870_sendStringToUser("Beginning closed window calibration.");
                calState = CAL_CLOSED;
                sampleCount = 0;
            }
            break;
        }
        case CAL_CLOSED:
        {                    
            if (windowAlarm_isMagneticOverflow(rawResult))
            {
                RN4870_sendStringToUser("[ERR] Magnetic Field is too strong. Please retry.");
                calState = CAL_CLOSED_WAIT;
            }

            sampleCount++;
            
            //Accumulate results
            averageX += (rawResult->X_Meas - offsetX);
            averageY += (rawResult->Y_Meas - offsetY);
            averageZ += (rawResult->Z_Meas - offsetZ);
            
            if (sampleCount >= MAGNETOMETER_CALIBRATION_SAMPLES)
            {
                //Compute Average
                averageX = averageX >> MAGNETOMETER_RSHIFT_SAMPLE_COUNT;
                averageY = averageY >> MAGNETOMETER_RSHIFT_SAMPLE_COUNT;
                averageZ = averageZ >> MAGNETOMETER_RSHIFT_SAMPLE_COUNT;
                
                //Compute Scaling Factor
                scaleX = _windowAlarm_computeScalingFactor(averageX);
                scaleY = _windowAlarm_computeScalingFactor(averageY);
                scaleZ = _windowAlarm_computeScalingFactor(averageZ);
                
                sprintf(USB_getCharBuffer(), "Scaling Completed. X_scale  = 2^%d, Y_scale = 2^%d, Z_scale = 2^%d\r\n",
                        scaleX, scaleY, scaleZ);
                USB_sendBufferedString();

                //Advance State Machine
                calState = CAL_CRACKED_WAIT;
                
                //Reset maximum value
                maxV = 0;
                
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
                USB_sendStringWithEndline("[ERR] Sensor value is below noise margin, please close the window and press the button to retry.");
                calState = CAL_CRACKED_ERR;
            }
            else if (windowAlarm_isMagneticOverflow(rawResult))
            {
                RN4870_sendStringToUser("[ERR] Magnetic Field is too strong. Please retry.");
                calState = CAL_CRACKED_ERR;
            }
            else if (buttonPressed)
            {
                USB_sendStringWithEndline("Starting window threshold calibration.");
                RN4870_sendStringToUser("Beginning window threshold calibration.");
                
                //Init Variables
                sampleCount = 0;
                averageR2 = 0;
                
                //Update State
                calState = CAL_CRACKED;
            }
            break;
        }
        case CAL_CRACKED_ERR:
        {
            if (buttonPressed)
            {                
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
            if (windowAlarm_isMagneticOverflow(rawResult))
            {
                RN4870_sendStringToUser("[ERR] Magnetic Field is too strong. Please retry.");
                calState = CAL_CRACKED_ERR;
            }
            
            //Increment Counter
            sampleCount++;
            
            //Accumulate Values
            averageR2 += normResults->r2;
            
            if (sampleCount >= MAGNETOMETER_CALIBRATION_SAMPLES)
            {
                //Set Cracked Threshold
                crackedV = averageR2 >> MAGNETOMETER_RSHIFT_SAMPLE_COUNT;

                //Warning for Low Noise Margin
                if (crackedV < MAGNETOMETER_NOISE_MARGIN)
                {
                    USB_sendStringWithEndline("[WARN] Threshold is below noise margin. Sensor may not operate correctly.");
                }
                
                //Apply Tolerance, if set
#ifdef MAGNETOMETER_VECTOR_TOLERANCE
                //For the window cracked threshold, use a <1 multiplier
                crackedV = round(crackedV * (1.0 - MAGNETOMETER_VECTOR_TOLERANCE));
                
                //For max magnitude, use a >1 multiplier
                maxV = round(maxV * (1.0 + MAGNETOMETER_VECTOR_TOLERANCE));
#endif
                
                //Print Threshold
                sprintf(USB_getCharBuffer(), "Threshold calibration complete. Threshold: %lu\r\n", crackedV);
                USB_sendBufferedString();
                                                
                //Update Cal State
                calState = CAL_CLOSED_FINAL_WAIT;
                
                //Clear Alarm Counter
                weightedAlarmState = 0;
            }
            break;
        }
        case CAL_CLOSED_FINAL_WAIT:
        {
            //Keep Track of Alarm Status
            if (windowAlarm_compareResults(normResults))
            {
                if (weightedAlarmState > 0)
                {
                    weightedAlarmState--;
                }
            }
            else
            {
                if (weightedAlarmState < MAGNETOMETER_ALARM_TRIGGER_MAX)
                {
                    weightedAlarmState++;
                }
            }
            
            //If the alarm would go off, then restart calibration
            if (weightedAlarmState >= MAGNETOMETER_ALARM_TRIGGER)
            {
                //Alarm went off, calibration failed
                RN4870_sendStringToUser("[ERROR] Alarm Calibration Failed, please retry.");
                
                //Return to step 1
                calState = CAL_OPEN_WAIT;
            }
            
            else if (buttonPressed)
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
                sprintf(USB_getCharBuffer(), "Allowed Angles: %d < XY < %d, %d < XZ < %d, %d < YZ < %d\r\n",
                        minXY, maxXY, minXZ, maxXZ, minYZ, maxYZ);
                USB_sendBufferedString();
#endif
                USB_sendStringWithEndline("Calibration complete.");
                RN4870_sendStringToUser("Calibration Complete.");
                calState = CAL_DEINIT;
                
                //Save values to EEPROM
                windowAlarm_saveThresholds();
                
                //Reset Alarm State Monitor
                alarmState = 0;
            }
            break;
        }
        case CAL_DEINIT:
        {
            if (MLX90392_setOperatingMode(MAGNETOMETER_ACTIVE_SAMPLE_RATE))
            {
                calState = CAL_GOOD;
                BLE_SW_Timer_reset();
            }
            else
            {
                USB_sendStringWithEndline("[ERR] Failed to change MLX90392 operating mode.");
            }
            break;
        }
        case CAL_GOOD:
        {
            //Move to bad state
            calState = CAL_BAD;
            break;
        }
    }
    
    //Instruction Printing State Machine
    if (oldState != calState)
    {
        alarmResultsReady = true;
        
        //Update old state
        oldState = calState;
    }
}

void windowAlarm_init(bool safeStart)
{
    bool success;
    
    //Set Debouncer State
    lastButtonState = WAKE_GetValue();
    
    //Print Welcome
    USB_sendStringRaw("Initializing MLX90392 Magnetometer Sensor...");
        
    //Init Sensor
    success = MLX90392_init();

    //If unable to init...
    if (!success)
    {
        USB_sendStringWithEndline("FAILED");
        magState = MAGNETOMETER_ERROR;
        return;
    }
    
    //If unable to init EEPROM constants
    if (!windowAlarm_loadFromEEPROM(safeStart))
    {
        windowAlarm_loadSettings(false);
        USB_sendStringWithEndline("CAL BAD");
        return;
    }
    
    USB_sendStringWithEndline("OK");
}

//Loads settings for window alarm
//If reset = true, settings are reset to defaults
void windowAlarm_loadSettings(bool nReset)
{
    if (!nReset)
    {
        calState = CAL_BAD;
    }
    else
    {
        //Window Vector Threshold
        crackedV = eeprom_read_dword((uint32_t*) CRACKED_THRESHOLD_V);
        
        //Max Allowed Vector Strength
        maxV = eeprom_read_dword((uint32_t*) MAX_VALUE_V);
        
        //Offsets
        offsetX = eeprom_read_word((uint16_t*) MAGNETOMETER_OFFSET_X);
        offsetY = eeprom_read_word((uint16_t*) MAGNETOMETER_OFFSET_Y);
        offsetZ = eeprom_read_word((uint16_t*) MAGNETOMETER_OFFSET_Z);
        
        //Scaling Values
        scaleX = eeprom_read_byte((uint8_t*) MAGNETOMETER_SCALER_X);
        scaleY = eeprom_read_byte((uint8_t*) MAGNETOMETER_SCALER_Y);
        scaleZ = eeprom_read_byte((uint8_t*) MAGNETOMETER_SCALER_Z);
        
#ifdef MAGNETOMETER_ANGLE_CHECK
        
        //Angle Ranges
        minXY = eeprom_read_word((uint16_t*) MAGNETOMETER_MIN_XY);
        maxXY = eeprom_read_word((uint16_t*) MAGNETOMETER_MAX_XY);
        minXZ = eeprom_read_word((uint16_t*) MAGNETOMETER_MIN_XZ);
        maxXZ = eeprom_read_word((uint16_t*) MAGNETOMETER_MAX_XZ);
        minYZ = eeprom_read_word((uint16_t*) MAGNETOMETER_MIN_YZ);
        maxYZ = eeprom_read_word((uint16_t*) MAGNETOMETER_MAX_YZ);
        
#endif      
        calState = CAL_GOOD;

    }
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
    
    uint8_t EEPROM_id_test = eeprom_read_byte((uint8_t*) EEPROM_MLX90392_ID);
    
    if ((!safeStart) && (EEPROM_id_test == _verify_sensorID))
    {       
        //EEPROM is valid, load thresholds
        windowAlarm_loadSettings(true);
        
        return true;
    }
    return false;
}

//Returns true if a magnetic overflow has occurred
bool windowAlarm_isMagneticOverflow(MLX90392_RawResult16* raw)
{
    uint8_t status2 = raw->status2.status2_byte;
    if (status2 & MLX90392_STAT2_HOVF_bm)
    {
        //Overflow!
        return true;
    }
    return false;
}

//Converts raw results into a normalized compressed value
void windowAlarm_createNormalizedResults(MLX90392_RawResult16* raw, MLX90392_NormalizedResults8* results)
{
    if (windowAlarm_isMagneticOverflow(raw))
    {
        USB_sendStringWithEndline("[ERR] Magnetic Overflow detected while normalizing data.");

        //Set values to max
        results->x = 0xFF;
        results->y = 0xFF;
        results->z = 0xFF;
        results->r2 = 0xFFFFFFFF;
        return;
    }
    
    int16_t x, y, z;
    
    //Remove offsets from measurements
    x = raw->X_Meas - offsetX;
    y = raw->Y_Meas - offsetY;
    z = raw->Z_Meas - offsetZ;
    
    //Normalize Values
    
    if (scaleX < 0)
        x = -1 * (x >> (scaleX * -1));
    else
        x = x >> (scaleX);
    
    if (scaleY < 0)
        y = -1 * (y >> (scaleY * -1));
    else
        y = y >> (scaleY);
    
    if (scaleZ < 0)
        z = -1 * (z >> (scaleZ * -1));
    else
        z = z >> (scaleZ);
    
    //Calculate Vector Strength
    results->r2 = (uint32_t)(x * x) +  (uint32_t)(y * y) + (uint32_t)(z * z);
    
    //Check to see if x, y, and z fit within an INT8
    
    if (x > INT8_MAX)
        results->x = INT8_MAX;
    else if (x < INT8_MIN)
        results->x = INT8_MIN;
    else
        results->x = x;
    
    if (y > INT8_MAX)
        results->y = INT8_MAX;
    else if (y < INT8_MIN)
        results->y = INT8_MIN;
    else
        results->y = y;
        
    if (z > INT8_MAX)
        results->z = INT8_MAX;
    else if (z < INT8_MIN)
        results->z = INT8_MIN;
    else
        results->z = z;
    
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
#endif    
}

//Checks to see if the alarm should be triggered or not
bool windowAlarm_compareResults(MLX90392_NormalizedResults8* normResults)
{    
    if ((normResults->r2 >= crackedV) && (normResults->r2 <= maxV))
    {
        //Within Expected Intensity
#ifdef MAGNETOMETER_ANGLE_CHECK   
        bool alarmOK = false;
        
        //Check Angles
        if ((normResults->xyAngle > maxXY) || (normResults->xyAngle < minXY))
        {
            alarmOK = false;
#ifdef MAGNETOMETER_DEBUG_PRINT
            sprintf(USB_getCharBuffer(), "XY Range: %d < XY < %d, found %d\r\n", minXY, maxXY, normResults->xyAngle);
            USB_sendBufferedString();
            USB_sendStringRaw("<XY Angle Error> - ");
#endif 
        }
        else if ((normResults->xzAngle > maxXZ) || (normResults->xzAngle < minXZ))
        {
            alarmOK = false;
#ifdef MAGNETOMETER_DEBUG_PRINT
            sprintf(USB_getCharBuffer(), "XZ Range: %d < XZ < %d, found %d\r\n", minXZ, maxXZ, normResults->xzAngle);
            USB_sendBufferedString();
            USB_sendStringRaw("<XZ Angle Error> - ");
#endif 
        }
        else if ((normResults->xzAngle > maxXZ) || (normResults->xzAngle < minXZ))
        {
            alarmOK = false;
#ifdef MAGNETOMETER_DEBUG_PRINT
            sprintf(USB_getCharBuffer(), "YZ Range: %d < YZ < %d, found %d\r\n", minYZ, maxYZ, normResults->yzAngle);
            USB_sendBufferedString();
            USB_sendStringRaw("<YZ Angle Error> - ");
#endif 
        }
        else
        {
            alarmOK = true;
        }
        
        return alarmOK;
#else
        return true;
#endif
    }
    else
    {
#ifdef MAGNETOMETER_DEBUG_PRINT
        USB_sendStringWithEndline("[ERROR] Alarm Compare Failed");
        sprintf(USB_getCharBuffer(), "Max Magnitude Strength: %lu, Cracked Threshold: %lu, Current Value %lu\r\n", maxV, crackedV, normResults->r2);
        USB_sendBufferedString();
#endif 
    }
    
    return false;
}

void windowAlarm_compareAndProcessResults(MLX90392_NormalizedResults8* normResults)
{
    bool alarmOK = windowAlarm_compareResults(normResults);
    
    if (alarmOK)
    {
        if (alarmState != 0)
        {
            alarmState--;
        }
    }
    else
    {
        if (alarmState != MAGNETOMETER_ALARM_TRIGGER_MAX)
        {
            alarmState++;
        }
    }
        
    if ((alarmState == MAGNETOMETER_ALARM_TRIGGER))
    {
        //Set Print Flag
        alarmResultsReady = true;
    }
 }

//Returns true if the alarm is ready to print a message
bool windowAlarm_getResultStatus(void)
{
    return alarmResultsReady;
}

//Prints the state of the alarm
void windowAlarm_printResults(void)
{
    //Clear Flag
    alarmResultsReady = false;
    
    //Calibration is not good
    if (magState == MAGNETOMETER_ERROR)
    {
        RN4870_sendStringToUser("[ERR] Communication error with magnetometer. Power cycle device.");
    }
    else if (calState != CAL_GOOD)
    {
        //Print Instructions
        RN4870_sendStringToUser(calInstructions[calState]);
        return;
    }
        
    //Print Results
    if (alarmState >= MAGNETOMETER_ALARM_TRIGGER)
    {
        RN4870_sendStringToUser("Alarm BAD");
    }
    else
    {
        RN4870_sendStringToUser("Alarm OK");
    }
}

//Prints calibration constants to UART
void windowAlarm_printCalibration(void)
{
    sprintf(RN4870_getCharBuffer(), "Offset X,Y,Z: %d, %d, %d\r\n"
            "Normalizing Right-Shifts X,Y,Z: %d, %d, %d\r\n"
            "Cracked Threshold: %lu\r\nMax Magnitude: %lu\r\n",
            offsetX, offsetY, offsetZ, scaleX, scaleY, scaleZ, crackedV, maxV);
    RN4870_printBufferedString();
}

//If called, the next data processed will be printed to the RN4870
void windowAlarm_requestRawPrint(void)
{
    requestPrint = true;
}

//Saves current calibration parameters
bool windowAlarm_saveThresholds(void)
{    
    if (!MLX90392_getRegister(MLX90392_DEVICE_ID, &sensorID))
        return false;
        
    eeprom_write_dword((uint32_t*) CRACKED_THRESHOLD_V, crackedV);
    eeprom_write_dword((uint32_t*) MAX_VALUE_V, maxV);
    
    eeprom_write_word((uint16_t*) MAGNETOMETER_OFFSET_X, offsetX);
    eeprom_write_word((uint16_t*) MAGNETOMETER_OFFSET_Y, offsetY);
    eeprom_write_word((uint16_t*) MAGNETOMETER_OFFSET_Z, offsetZ);
    
    eeprom_write_byte((uint8_t*) MAGNETOMETER_SCALER_X, scaleX);
    eeprom_write_byte((uint8_t*) MAGNETOMETER_SCALER_Y, scaleY);
    eeprom_write_byte((uint8_t*) MAGNETOMETER_SCALER_Z, scaleZ);
    
#ifdef MAGNETOMETER_ANGLE_CHECK
    
    eeprom_write_word((uint16_t*) MAGNETOMETER_MIN_XY, minXY);
    eeprom_write_word((uint16_t*) MAGNETOMETER_MAX_XY, maxXY);
    eeprom_write_word((uint16_t*) MAGNETOMETER_MIN_XZ, minXZ);
    eeprom_write_word((uint16_t*) MAGNETOMETER_MAX_XZ, maxXZ);
    eeprom_write_word((uint16_t*) MAGNETOMETER_MIN_YZ, minYZ);
    eeprom_write_word((uint16_t*) MAGNETOMETER_MAX_YZ, maxYZ);
    
#endif
    
    //Write the ID of the sensor
    eeprom_write_byte((uint8_t*) EEPROM_MLX90392_ID, sensorID);

    return true;
}

//Returns true if calibration is good
bool windowAlarm_isCalGood(void)
{
    return (calState == CAL_GOOD);
}

//Returns true if the alarm is OK, false if the alarm was tripped
bool windowAlarm_isAlarmOK(void)
{   
    //If unable to boot magnetometer
    if (magState == MAGNETOMETER_ERROR)
    {
        return false;
    }
    
    //Print Results
    if (alarmState >= MAGNETOMETER_ALARM_TRIGGER)
    {
        return false;
    }
    
    return true;
}

void windowAlarm_FSM(void)
{    
    //Software button debouncer
    //Button is cleared at the end of MAGNETOMETER_WAIT
    if ((!lastButtonState) && (WAKE_GetValue()))
    {
        //Button is pressed
        lastButtonState = true;
        buttonPressed = true;
    }
    else if ((lastButtonState) && (!WAKE_GetValue()))
    {
        //Button released (reset state machine)
        lastButtonState = false;
    }
    
    if ((!MVIO_isOK()) && (magState != MAGNETOMETER_ERROR))
    {
        //MVIO Power Error
        magState = MAGNETOMETER_ERROR;
        
        //Triggers immediate message print
        //magCounter = MAGNETOMETER_ERROR_DELAY;
        
        //Set the results flag so the error is printed.
        alarmResultsReady = true;
        
        //Flush I2C
        TWI0_flush();
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
                MLX90392_RawResult16 magResult;
                MLX90392_NormalizedResults8 normResult;
                success = MLX90392_getResult(&magResult);
                if (success)
                {
                    windowAlarm_createNormalizedResults(&magResult, &normResult);
                    
                    if (requestPrint)
                    {
                        //Clear Flag
                        requestPrint = false;
                        
                        RN4870_sendStringToUser("--Begin Magnetometer Data--");
                        sprintf(RN4870_getCharBuffer(), "Raw X,Y,Z: %d, %d, %d\r\n"
                                "Normalized X,Y,Z: %d, %d, %d\r\n"
                                "Vector Strength: %lu\r\n",
                                magResult.X_Meas, magResult.Y_Meas, magResult.Z_Meas,
                                normResult.x, normResult.y, normResult.z, normResult.r2);
                        RN4870_printBufferedString();
                        RN4870_sendStringToUser("--End Magnetometer Data--");
                    }
                    
                    //If set, print the raw values of the magnetometer
#ifdef MAGNETOMETER_PRINT_CSV
                    sprintf(USB_getCharBuffer(), "%d, %d, %d\r\n", magResult.X_Meas, magResult.Y_Meas, magResult.Z_Meas);
                    USB_sendBufferedString();
#elif MAGNETOMETER_RAW_VALUE_PRINT 
                    sprintf(USB_getCharBuffer(), "<RAW Values>\r\nX: %d, Y: %d, Z: %d\r\n",
                        magResult.X_Meas, magResult.Y_Meas, magResult.Z_Meas); 
                    USB_sendBufferedString();
                    
                    sprintf(USB_getCharBuffer(), "<Normalized Values>\r\nX: %d, Y: %d, Z: %d, R^2: %lu\r\n",
                        normResult.x, normResult.y, normResult.z, normResult.r2);
                    USB_sendBufferedString();
#endif
                    if ((calState == CAL_GOOD) && (!buttonPressed))
                    {
                        //Process Result if Cal is valid
                        windowAlarm_compareAndProcessResults(&normResult);
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
                USB_sendStringWithEndline("Magnetometer Successfully Restarted");
                
                prevResultState = INVALID;
                
                //Attempt to load calibration constants from EEPROM
                windowAlarm_loadFromEEPROM(false);
                
                //Update state machine
                magState = MAGNETOMETER_START;
                //magCounter = MAGNETOMETER_ERROR_DELAY;
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
            //Retry Power-Up
            if (MVIO_isOK())
            {                
                //Update State
                magState = MAGNETOMETER_REINIT_WAIT;
            }
            
            //Simple delay to keep this error from filling the UART.
//            if (magCounter >= MAGNETOMETER_ERROR_DELAY)
//            {
//                RN4870_sendStringToUser("Magnetometer Sensor Error - Reboot Device");
//                magCounter = 1;
//            }
//            else
//            {
//                magCounter++;
//            }

            break;
        }
    }
}

//Updates the pushbutton state to prevent calibration when waking up
void windowAlarm_maskButton(void)
{
    lastButtonState = true;
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
        //magCounter = MAGNETOMETER_ERROR_DELAY;
    }
    
}