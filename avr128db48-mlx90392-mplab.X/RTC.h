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

#ifndef RTC_H
#define	RTC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
    
    //Configures the RTC and PIT
    void RTC_init(void);
    
    //Resets the RTC count back to 0
    void RTC_reset(void);
    
    //Set the OVF Callback Function
    void RTC_setOVFCallback(void (*func)(void));
    
    //Set the PIT Callback Function
    void RTC_setPITCallback(void (*func)(void));
    
    //Set the CMP Callback Function
    void RTC_setCMPCallback(void (*func)(void));

    //Returns the period of the RTC
    uint16_t RTC_getPeriod(void);
    
    //Returns the compare of the RTC
    uint16_t RTC_getCompare(void);
    
    //Returns the count of the RTC
    uint16_t RTC_getCount(void);
    
    //Sets the period of the RTC
    void RTC_setPeriod(uint16_t period);
    
    //Sets the compare of the RTC
    void RTC_setCompare(uint16_t comp);
    
    //Set the period of the PIT
    void RTC_setPIT(uint8_t period);
    
    //Returns true if PIT was triggered
    bool RTC_isPITTriggered(void);
    
    //Clears PIT Triggered Flag
    void RTC_clearPITTriggered(void);
    
    //Returns true if RTC OVF
    bool RTC_isOVFTriggered(void);
    
    //Clear RTC OVF Flag
    void RTC_clearOVFTrigger(void);
    
    //Returns true if RTC CMP
    bool RTC_isCMPTriggered(void);
    
    //Clear RTC CMP Flag
    void RTC_clearCMPTrigger(void);

    
#ifdef	__cplusplus
}
#endif

#endif	/* RTC_H */

