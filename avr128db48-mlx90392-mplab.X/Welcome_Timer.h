#ifndef WELCOME_TIMER_H
#define	WELCOME_TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>

//How many RTC cycles to wait before printing
#define WELCOME_DELAY_BLUETOOTH 327

    //Start the Welcome Timer
    void welcomeTimer_start(void);
    
    //Stop the Welcome Timer
    void welcomeTimer_stop(void);
    
    //Returns true if running, false if not
    bool welcomeTimer_isRunning(void);
    
    //Add the current delta from the RTC
    void welcomeTimer_addTime(void);
    
    //Sets the starting count of the RTC
    void welcomeTimer_setCurrentTime(void);
    
    //Clear Trigger Flag and Count
    void welcomeTimer_reset(void);
    
    //Returns true if the timer has triggered
    bool welcomeTimer_hasTriggered(void);
    
    
#ifdef	__cplusplus
}
#endif

#endif	/* WELCOME_TIMER_H */

