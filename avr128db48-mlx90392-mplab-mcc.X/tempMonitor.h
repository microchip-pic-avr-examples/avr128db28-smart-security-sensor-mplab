#ifndef TEMPMONITOR_H
#define	TEMPMONITOR_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdbool.h>
    
    //Init the Temp Monitor
    void tempMonitor_init(bool safeStart);
    
    //Run the Temp Monitor's Finite State Machine
    void tempMonitor_FSM(void);
    
    //Call this function, or attach to an ISR, to request a new conversion
    void tempMonitor_requestConversion(void);
    
    //Returns true if results are ready
    bool tempMonitor_getResultStatus(void);
    
    //Call this function to print the temp results to the Bluetooth UART Interface
    void tempMonitor_printResults(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* TEMPMONITOR_H */

