#include "windowAlarm.h"
#include "mcc_generated_files/system/system.h"
#include "MLX90392.h"
#include "EEPROM_Locations.h"
#include "EEPROM_Utility.h"

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
MAGNETOMETER_START = 0, MAGNETOMETER_WAIT, MAGNETOMETER_ERROR, 
        MAGNETOMETER_REINIT_WAIT, MAGNETOMETER_REINIT
} MagentometerMeasState;

typedef enum{
    NO_CAL = 0, CAL_CLOSED, CAL_CRACKED, CAL_OPEN, CAL_GOOD
} CalibrationStatus;

//Calibration Status
static CalibrationStatus calState = NO_CAL;

//State Machine Variables
static volatile MagentometerMeasState magState = MAGNETOMETER_START;
static volatile uint16_t magCounter = MAGNETOMETER_ERROR_DELAY;
static volatile bool buttonPressed = false;

//Thresholds
static volatile int16_t X_min_closed, X_max_closed, Y_min_closed, Y_max_closed, 
                Z_min_closed, Z_max_closed;

static volatile int16_t X_min_cracked, X_max_cracked, Y_min_cracked, Y_max_cracked, 
                Z_min_cracked, Z_max_cracked;

static uint8_t sensorID = 0x00;


void windowAlarm_init(bool safeStart)
{
    bool success;

    printf("Initializing MLX90392 Magnetometer Sensor...");
    success = MLX90392_init();

    //If unable to init...
    if (!success)
    {
        printf("FAILED\r\n");
        magState = MAGNETOMETER_ERROR;
        return;
    }
    
    uint8_t _verify_sensorID;
    success = MLX90392_getRegister(MLX90392_DEVICE_ID, &_verify_sensorID);
    
    //If unable to get ID
    if (!success)
    {
        printf("FAILED\r\n");
        magState = MAGNETOMETER_ERROR;
        return;
    }
    
    uint8_t EEPROM_id_test = EEPROM_Read(EEPROM_MLX90392_ID);
    
    if ((!safeStart) && (EEPROM_id_test == _verify_sensorID))
    {       
        //EEPROM is valid, load thresholds
        X_max_closed = EEPROM_Read(CLOSED_THRESHOLD_X_MAX);
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
        Z_min_cracked = EEPROM_Read(CRACKED_THRESHOLD_Z_MIN);
        
        calState = CAL_GOOD;
    }
    else
    {
        //EEPROM is not valid
        calState = NO_CAL;
    }
    
    
    
    printf("OK\r\n");
}

//Saves current thresholds
bool windowAlarm_saveThresholds(void)
{
    if (!MLX90392_getRegister(MLX90392_DEVICE_ID, &sensorID))
        return false;
        
    save16BToEEPROM(CLOSED_THRESHOLD_X_MAX, (uint16_t) X_max_closed);
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
    save16BToEEPROM(CRACKED_THRESHOLD_Z_MIN, (uint16_t) Z_min_cracked);
    
    //Write the ID of the sensor
    EEPROM_Write(EEPROM_MLX90392_ID, sensorID);

    return true;
}

void windowAlarm_FSM(void)
{
    if (buttonPressed)
    {
        X_max_closed = 0;
        X_min_closed = 1;
        Y_max_closed = 2;
        Y_min_closed = 3;
        Z_max_closed = 4;
        Z_min_closed = 5;

        X_max_cracked = 6;
        X_min_cracked = 7;
        Y_max_cracked = 8;
        Y_min_cracked = 9;
        Z_max_cracked = 10;
        Z_min_cracked = 11;
        
        windowAlarm_saveThresholds();
        buttonPressed = false;
    }
    
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
                MLX90392_Result magResult;
                success = MLX90392_getResult(&magResult);
                if (success)
                {
                    printf("X: %d, Y: %d, Z: %d\r\n", magResult.X_Meas, magResult.Y_Meas, magResult.Z_Meas);
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
                printf("Magnetometer Sensor Error - Reboot Device\r\n");
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
    buttonPressed = true;
}