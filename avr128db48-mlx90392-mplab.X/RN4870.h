#ifndef RN4870_H
#define	RN4870_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
//Flipped version of AOK... saves some computation time
#define RN4870_AOK "KOA"
    
    //Configures the RN4870
    void RN4870_init(void);
    
    //Checks for events from the RN4870
    void RN4870_checkForEvents(void);
    
    //Powers up the RN4870. Non-Blocking
    void RN4870_powerUp(void);
    
    //Powers down the RN4870. Non-Blocking
    void RN4870_powerDown(void);
    
    //Enters Command Mode in the RN4870
    bool RN4870_enterCommandMode(void);
    
    //Exits Command Mode in the RN4870
    void RN4870_exitCommandMode(void);
    
    //Sends a Command to the RN4870
    bool RN4870_sendCommand(const char* string, uint8_t timeout);
    
    //Sends a Command to the RN4870, and prints a DBG log
    void RN4870_sendCommandAndPrint(const char* string, uint8_t timeout);
    
    //Returns true if ready to send data
    bool RN4870_isReady(void);
    
    //Sends a string to the user, if powered up.
    void RN4870_sendStringToUser(const char* str);

    //Returns the char buffer associated with the BLE
    char* RN4870_getCharBuffer(void);
    
    //Sends the cached string to the user, if powered up
    void RN4870_printBufferedString(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* RN4870_H */

