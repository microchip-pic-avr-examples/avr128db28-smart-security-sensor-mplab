#ifndef TCB0_ONESHOT_H
#define	TCB0_ONESHOT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
    
    //Configures TCB0 as a 1 shot timer with a period of 1 ms
    //Triggered by EVSYS A
    void TCB0_init(void);

    //Returns true if TCB is currently running
    bool TCB0_isRunning(void);
    
    //Triggers the timer
    void TCB0_triggerTimer(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TCB0_ONESHOT_H */

