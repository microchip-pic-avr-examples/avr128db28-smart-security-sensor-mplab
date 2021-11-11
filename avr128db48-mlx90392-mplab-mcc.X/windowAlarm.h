#ifndef WINDOWALARM_H
#define	WINDOWALARM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
    
//How many cycles of the PIT to run before reprinting magnetometer error
#define MAGNETOMETER_ERROR_DELAY 1250
    
    typedef enum {
        WINDOW_CLOSED = 0, WINDOW_CRACKED, WINDOW_OPEN
    } MagneticAlarmState;
    
    //Init the Magnetometer and related parameters
    void windowAlarm_init(bool safeStart);
    
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

