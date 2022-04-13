#ifndef DEMO_H
#define	DEMO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
    
//Amount of memory to allocate for input parameters
#define DEMO_PARAM_LENGTH 16
    
//3s Period
#define DEMO_SAMPLE_RATE_FAST 0x0300
//15s Period
#define DEMO_SAMPLE_RATE_NORM 0x0F00
//30s Period
#define DEMO_SAMPLE_RATE_SLOW 0x1E00
    
//Time for Temp Sensor to Run (320 = 1.25s)
#define DEMO_TEMP_DELAY_START 320
    
    //Load settings in EEPROM or reset to default (if in safe mode)
    void DEMO_init(bool safeStart);
    
    //Loads settings for the demo
    //If nReset = false, will load default settings
    void DEMO_loadSettings(bool nReset);
    
    //Handle User Commands
    bool DEMO_handleUserCommands(void);

    //Sets the update rate of the demo
    void DEMO_setSystemUpdateRateEEPROM(uint16_t rate);
    
    //Sets the update rate of the demo
    //Does NOT write to EEPROM
    void DEMO_setSystemUpdateRateRAM(uint16_t rate);
    
#ifdef	__cplusplus
}
#endif

#endif	/* DEMO_H */

