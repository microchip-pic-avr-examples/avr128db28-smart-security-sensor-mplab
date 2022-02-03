#ifndef DEMO_H
#define	DEMO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
    
//3s Period
#define DEMO_SAMPLE_RATE_FAST 0x0300
    
//15s Period
#define DEMO_SAMPLE_RATE_NORM 0x0F00
    
//30s Period
#define DEMO_SAMPLE_RATE_SLOW 0x1000
    
    //Handle User Commands
    bool DEMO_handleUserCommands(void);

#ifdef	__cplusplus
}
#endif

#endif	/* DEMO_H */

