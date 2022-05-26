/*
© [2022] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

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
    
//If defined, this will print values in CSV format
//#define MAGNETOMETER_PRINT_CSV
    
//If defined, this will print a formatted string containing the magnetometer raw values
//#define MAGNETOMETER_RAW_VALUE_PRINT
             
//If defined, this will print extra debug info
//#define MAGNETOMETER_DEBUG_PRINT
        
//How many cycles of the PIT to run before reprinting magnetometer error
#define MAGNETOMETER_ERROR_DELAY 1250

//How many samples to use for MAX / MIN comparisons
#define MAGNETOMETER_CALIBRATION_SAMPLES 1024
    
//How many right-shifts for the acquired average
#define MAGNETOMETER_RSHIFT_SAMPLE_COUNT 10
        
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
#define MAGNETOMETER_VECTOR_TOLERANCE 0.15

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

