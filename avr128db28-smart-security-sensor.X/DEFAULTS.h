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

#ifndef DEFAULTS_H
#define	DEFAULTS_H

#ifdef	__cplusplus
extern "C" {
#endif

//Default Result Rate (15s))
#define DEFAULT_RTC_PERIOD 0x0F00
        
//Default Bluetooth Idle Time (30s)
#define DEFAULT_BLUETOOTH_IDLE_PERIOD 0x1E00
    
//Default Temp Unit
#define DEFAULT_TEMP_UNIT 'C'
    
//Default High Temp Warning (90F/32.2C). Value must be in C
#define DEFAULT_TEMP_WARNING_H 32.2

//Default High Temp Warning (50F/10C). Value must be in C
#define DEFAULT_TEMP_WARNING_L 10.0

#ifdef	__cplusplus
}
#endif

#endif	/* DEFAULTS_H */

