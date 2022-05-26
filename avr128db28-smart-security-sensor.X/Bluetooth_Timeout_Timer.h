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
    
    //Prints user settings to UART
    void BLE_SW_Timer_printUserSettings(void);
    
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

