#ifndef WINDOWALARM_H
#define	WINDOWALARM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "MLX90392.h"
    
    typedef struct {
        int8_t x;           //Compressed X
        int8_t y;           //Compressed Y
        int8_t z;           //Compressed Z
        
#ifdef MAGNETOMETER_ANGLE_CHECK
        //Ratios of X/Y, X/Z, Y/Z. Tan([Angle]) = <Ratio>
        int16_t xyAngle;     //X/Y
        int16_t xzAngle;     //X/Z
        int16_t yzAngle;     //Y/Z
#endif
        
        uint32_t r2;        //Vector Sum of Compressed Values
    } MLX90392_NormalizedResults8;
    
#define RTC_PIT_FAST_TIMING RTC_PERIOD_CYC128_gc
    
#define RTC_PIT_NORMAL_TIMING RTC_PERIOD_CYC2048_gc
    
    //Init the Magnetometer and related parameters
    void windowAlarm_init(bool safeStart);
    
    //Loads settings for window alarm
    //If nReset = false, settings are reset to defaults
    void windowAlarm_loadSettings(bool nReset);
    
    //Tries to load constants from EEPROM - called by windowAlarm_init
    //Returns true if successful, or false if EEPROM is invalid
    bool windowAlarm_loadFromEEPROM(bool safeStart);
    
    //Starts a calibration sequence, but does NOT interrupt it.
    void windowAlarm_requestCalibration(void);
    
    //Internal function for setting the trigger thresholds (calibration)
    void windowAlarm_runCalibration(MLX90392_RawResult16* result, MLX90392_NormalizedResults8* normResults);
    
    //Checks to see if the alarm should be triggered or not
    bool windowAlarm_compareResults(MLX90392_NormalizedResults8* normResults);
    
    //Process Data from Magnetometer and trigger alarm, if needed
    void windowAlarm_compareAndProcessResults(MLX90392_NormalizedResults8* results);
    
    //Returns true if the alarm is ready to print a message
    bool windowAlarm_getResultStatus(void);
    
    //Prints the state of the alarm
    void windowAlarm_printResults(void);
    
    //Converts raw results into a normalized compressed value
    void windowAlarm_createNormalizedResults(MLX90392_RawResult16* raw, MLX90392_NormalizedResults8* results);
    
    //Saves current thresholds.
    bool windowAlarm_saveThresholds(void);
    
    //Returns true if calibration is good
    bool windowAlarm_isCalGood(void);
    
    //Returns true if the alarm is OK, false if the alarm was tripped
    bool windowAlarm_isAlarmOK(void);
        
    //Run the Finite State Machine for the Magnetometer
    void windowAlarm_FSM(void);
    
    //Updates the pushbutton state to prevent calibration when waking up
    void windowAlarm_maskButton(void);
    
    //Run this ISR if the MVIO changes readiness
    void _windowAlarm_onMVIOChange(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* WINDOWALARM_H */

