#ifndef RTC_H
#define	RTC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
    
    //Configures the RTC and PIT
    void RTC_init(void);
    
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

