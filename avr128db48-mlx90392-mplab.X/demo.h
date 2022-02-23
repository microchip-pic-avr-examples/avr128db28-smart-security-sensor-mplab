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
    
    //Handle User Commands
    bool DEMO_handleUserCommands(void);

#ifdef	__cplusplus
}
#endif

#endif	/* DEMO_H */

