#ifndef RN4870_H
#define	RN4870_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
//Flipped version of AOK... saves some computation time
#define RN4870_AOK "KOA"
    
    void RN4870_init(void);
    
    bool RN4870_enterCommandMode(void);
    void RN4870_exitCommandMode(void);
    
    bool RN4870_sendCommand(const char* string, uint8_t timeout);
    void RN4870_sendCommandAndPrint(const char* string, uint8_t timeout);

#ifdef	__cplusplus
}
#endif

#endif	/* RN4870_H */

