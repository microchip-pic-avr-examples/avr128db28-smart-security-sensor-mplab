#ifndef BLUETOOTH_TIMEOUT_TIMER_H
#define	BLUETOOTH_TIMEOUT_TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>

    //Load the Timeout Settings
    //If nReset = false, will load default settings
    void BLE_SW_Timer_loadSettings(bool nReset);
    
    //Sets and stores the current period to EEPROM
    void BLE_SW_Timer_saveSettings(uint16_t period);
    
    //Set the number of cycles (from the RTC)
    void BLE_SW_Timer_setPeriod(uint16_t period);

    //Add the current delta from the RTC
    void BLE_SW_Timer_addTime(void);
    
    //Sets the starting count of the RTC
    void BLE_SW_Timer_setCurrentTime(void);
    
    //Resets the BLE_SW_Timer
    //Clear Trigger Flag and Count
    void BLE_SW_Timer_reset(void);
    
    //Returns true if the timer has triggered
    bool BLE_SW_Timer_hasTriggered(void);
    
    
#ifdef	__cplusplus
}
#endif

#endif	/* BLUETOOTH_TIMEOUT_TIMER_H */

