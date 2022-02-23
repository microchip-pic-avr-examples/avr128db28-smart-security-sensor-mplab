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

    //Returns the period of the RTC
    uint16_t RTC_getPeriod(void);
    
    //Sets the period of the RTC
    void RTC_setPeriod(uint16_t period);
    
    //Returns true if PIT was triggered
    bool RTC_isPITTriggered(void);
    
    //Clears PIT Triggered Flag
    void RTC_clearPITTriggered(void);
    
    //Returns true if RTC was triggered
    bool RTC_isRTCTriggered(void);
    
    //Clears RTC Triggered Flag
    void RTC_clearRTCTrigger(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* RTC_H */

