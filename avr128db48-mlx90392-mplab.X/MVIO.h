#ifndef MVIO_H
#define	MVIO_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdbool.h>
    
    //Initializes the MVIO
    void MVIO_init(void);
    
    //Sets the callback function for MVIO Change Events
    void MVIO_setCallback(void(*func)(void));
    
    //Returns the current state of the MVIO
    bool MVIO_isOK(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* MVIO_H */

