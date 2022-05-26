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
    
    //Sets the Red/Green LEDs according to the current state
    //Also, resets the BLE Timer, if Alarm / Cal is Active
    void DEMO_setAlarmLEDs(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* DEMO_H */

