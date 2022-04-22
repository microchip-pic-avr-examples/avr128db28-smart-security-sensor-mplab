#ifndef WINDOWALARM_CONFIG_H
#define	WINDOWALARM_CONFIG_H

#include "MLX90392.h"


#ifdef	__cplusplus
extern "C" {
#endif

//Sets the default sample rate in calibration mode
#define MAGNETOMETER_CAL_SAMPLE_RATE CONT_200HZ

//Sets the sample rate when running normally
#define MAGNETOMETER_ACTIVE_SAMPLE_RATE CONT_10HZ

//How often should the instructions be repeated? (Measured in loop iterations)
#define INSTR_REPEAT_ITERATIONS 800
    
//If defined, this will print values in CSV format
//#define MAGNETOMETER_PRINT_CSV
    
//If defined, this will print a formatted string containing the magnetometer raw values
//#define MAGNETOMETER_RAW_VALUE_PRINT
             
//If defined, this will print extra debug info
//#define MAGNETOMETER_DEBUG_PRINT
        
//How many cycles of the PIT to run before reprinting magnetometer error
#define MAGNETOMETER_ERROR_DELAY 1250

//How many samples to use for MAX / MIN comparisons
#define MAGNETOMETER_CALIBRATION_SAMPLES 1000
        
//Defines the number of events to trigger the alarm
#define MAGNETOMETER_ALARM_TRIGGER 10
    
//Defines the highest range for the alarm
#define MAGNETOMETER_ALARM_TRIGGER_MAX 20

//Defines the number of cycles to run between printing alarm status. Ignored at trigger thrshold
#define MAGNETOMETER_ALARM_PRINT_RATE 20
    
//Defines the vector noise margin
#define MAGNETOMETER_NOISE_MARGIN 40
    
//Comment out this define to disable
//Magnitude Tolerance for Alarm (1 + (% Value))
#define MAGNETOMETER_VECTOR_TOLERANCE 0.1

//If defined, the angles of the magnetometer will be computed and used to monitor for tampering.
//#define MAGNETOMETER_ANGLE_CHECK
    
//Pre-Scaler for Computing Angles (X/Y, X/Z, Y/Z)
#define MAGNETOMETER_ANGLE_SCALE 100
    
//Comment out this define to disable
//Angular Position Tolerance for Alarm (# of bits +/- this value)
#define MAGNETOMETER_ANGLE_TOLERANCE 5

#ifdef	__cplusplus
}
#endif

#endif	/* WINDOWALARM_CONFIG_H */

