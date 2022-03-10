#ifndef TEMPMONITOR_H
#define	TEMPMONITOR_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdbool.h>
#include <stdint.h>
        
//Default High Temp Warning (90F/32.2C). Value must be in C
#define DEFAULT_TEMP_WARNING_H 32.2

//Default High Temp Warning (50F/10C). Value must be in C
#define DEFAULT_TEMP_WARNING_L 10.0

    //Init the Temp Monitor
    void tempMonitor_init(bool safeStart);
    
    //Load Temp. Sensor Settings and Constants
    //If nReset = false, defaults will be loaded
    void tempMonitor_loadSettings(bool nReset);
    
    //Sets the temperature unit for the demo. C - Celsius (default), F - Fahrenheit, K - Kelvin
    void tempMonitor_setUnit(char unit);
    
    //Updates the RTC's sample rate and stores it in EEPROM
    void tempMonitor_updateSampleRate(uint16_t sampleRate);
    
    //Run the Temp Monitor's Finite State Machine
    void tempMonitor_FSM(void);
    
    //Call this function, or attach to an ISR, to request a new conversion
    void tempMonitor_requestConversion(void);
    
    //Returns true if results are ready
    bool tempMonitor_getResultStatus(void);
    
    //Call this function to print the temp results to the Bluetooth UART Interface
    void tempMonitor_printResults(void);
    
    //Sets the warning temp for high temperatures. Temp units are auto-converted from current set to C 
    void tempMonitor_setTempWarningHigh(float temp);
    
    //Sets the warning temp for low temperatures. Temp units are auto-converted from current set to C 
    void tempMonitor_setTempWarningLow(float temp);
    
#ifdef	__cplusplus
}
#endif

#endif	/* TEMPMONITOR_H */

