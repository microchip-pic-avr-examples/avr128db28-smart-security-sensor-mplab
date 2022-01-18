#ifndef RN4870_RX_H
#define	RN4870_RX_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
//Size of String Buffer
#define RN4870_RX_BUFFER_SIZE 256
    
//Delimiters
#define RN4870_DELIM_STATUS '%'
#define RN4870_DELIM_RESP '\r'   
#define RN4870_MARKER_CMD '>'
    
    //Initializes the RX Engine for the RN4870
    void RN4870RX_init(void);
    
    //Insert a character into the buffer
    void RN4870RX_loadCharacter(char input);
    
    //Returns true if a deliminater was received.
    bool RN4870RX_isResponseComplete(void);
    
    //Returns true if one of the chars from CMD has been received.
    bool RN4870RX_isCMDPresent(void);
    
    //Clears the flag set if one of the chars from CMD has been received
    void RN4870RX_clearCMDFlag(void);
    
    //Clears the response buffer
    void RN4870RX_clearResponseBuffer(void);
    
    //Refreshes the response buffer with the last 3 chars prior to deliminater
    void RN4870RX_loadResponseBuffer(void);
    
    //Discards the buffer
    void RN4870RX_clearBuffer(void);
    
    //Waits for a message to be received and checks to see if it matches string.
    //Timeout is specified in milliseconds
    bool RN4870RX_waitForResponseRX(uint16_t timeout, const char* compare);
    
    //Waits for a message to be received and checks to see if it matches string.
    //Timeout is specified in milliseconds
    bool RN4870RX_waitForCommandRX(uint16_t timeout);

    
#ifdef	__cplusplus
}
#endif

#endif	/* RN4870_RX_H */

