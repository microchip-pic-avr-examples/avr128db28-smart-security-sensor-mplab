#ifndef TEMPMONITOR_H
#define	TEMPMONITOR_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdbool.h>
#include <stdint.h>
        
    //Init the Temp Monitor
    void tempMonitor_init(bool safeStart);
    
    //Load Temp. Sensor Settings and Constants
    //If nReset = false, defaults will be loaded
    void tempMonitor_loadSettings(bool nReset);
    
    //Sets the temperature unit for the demo. C - Celsius (default), F - Fahrenheit, K - Kelvin
    bool tempMonitor_setUnit(char unit);
    
    //Returns the current temperature unit
    void tempMonitor_printUserSettings(void);
        
    //Sets whether the sensor can run in sleep
    void tempMonitor_setRunInSleep(bool enable);
    
    //Returns true if the sensor can run in sleep
    bool tempMonitor_getRunInSleep(void);
    
    //Run the Temp Monitor's Finite State Machine
    void tempMonitor_FSM(void);
    
    //Call this function, or attach to an ISR, to request a new conversion
    void tempMonitor_requestConversion(void);
    
    //Returns true if results are ready
    bool tempMonitor_getResultStatus(void);
    
    //Call this function to print the temp results to the Bluetooth UART Interface
    void tempMonitor_printResults(void);
    
    //Returns true if the temp is abnormal
    bool tempMonitor_isTempNormal(void);
        
    //Prints the last results to the Bluetooth Interface. Does NOT clear flags or check for new data
    void tempMonitor_printLastResults(void);
        
    //Sets the warning temp for high temperatures. Temp units are auto-converted from current set to C 
    void tempMonitor_setTempWarningHigh(float temp);
    
    //Returns the warning temp for low temperatures. Temp units are auto-converted from C to current
    float tempMonitor_getTempWarningHigh(void);
    
    //Sets the warning temp for low temperatures. Temp units are auto-converted from current set to C 
    void tempMonitor_setTempWarningLow(float temp);
    
    //Returns the warning temp for low temperatures. Temp units are auto-converted from C to current
    float tempMonitor_getTempWarningLow(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* TEMPMONITOR_H */

