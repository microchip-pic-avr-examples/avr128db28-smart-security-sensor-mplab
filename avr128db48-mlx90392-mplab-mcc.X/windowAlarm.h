#ifndef WINDOWALARM_H
#define	WINDOWALARM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "MLX90392.h"
    
//How many cycles of the PIT to run before reprinting magnetometer error
#define MAGNETOMETER_ERROR_DELAY 1250
    
//How many samples to use for MAX / MIN comparisons
#define MAGNETOMETER_CALIBRATION_SAMPLES 1000
    
//Pre-Scaler for Computing Angles (X/Y, X/Z, Y/Z)
#define MAGNETOMETER_ANGLE_SCALE 10
    
//Defines the vector threshold to begin computing angles
#define MAGNETOMETER_ANGLE_THRESHOLD 40
    
//If defined, this will print values in CSV format
//#define MAGNETOMETER_PRINT_CSV
    
//If defined, this will print a formatted string containing the magnetometer raw values
//#define MAGNETOMETER_RAW_VALUE_PRINT
     
//If defined, this will print extra debug info
#define MAGNETOMETER_DEBUG_PRINT
    
    typedef struct {
        int8_t x;           //Compressed X
        int8_t y;           //Compressed Y
        int8_t z;           //Compressed Z
        
        //Ratios of X/Y, X/Z, Y/Z. Tan([Angle]) = <Ratio>
        int8_t xyAngle;     //X/Y
        int8_t xzAngle;     //X/Z
        int8_t yzAngle;     //Y/Z
        
        uint32_t r2;        //Vector Sum of Compressed Values
    } MLX90392_NormalizedResults;
    
    //Init the Magnetometer and related parameters
    void windowAlarm_init(bool safeStart);
    
    //Converts raw results into a normalized compressed value
    void windowAlarm_createNormalizedResults(MLX90392_RawResult* raw, MLX90392_NormalizedResults* results);
    
    //Saves current thresholds.
    bool windowAlarm_saveThresholds(void);
    
    //Run the Finite State Machine for the Magnetometer
    void windowAlarm_FSM(void);
    
    //Run this ISR if the MVIO changes readiness
    void _windowAlarm_onMVIOChange(void);
    
    //Run this ISR if the pushbutton is pressed (from debouncer)
    void _windowAlarm_buttonPressed(void);

#ifdef	__cplusplus
}
#endif

#endif	/* WINDOWALARM_H */

