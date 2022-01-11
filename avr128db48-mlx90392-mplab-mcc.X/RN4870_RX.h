#ifndef RN4870_RX_H
#define	RN4870_RX_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
    //Size of String Buffer
#define RN4870_RX_BUFFER_SIZE 255
    
    //Delimiters
#define RN4870_DELIM_STATUS '%'
#define RN4870_DELIM_RESP '\r'        
    
    //Initializes the RX Engine for the RN4870
    void RN4870RX_init(void);
    
    //Insert a character into the buffer
    void RN4870RX_loadCharacter(char input);
    
    //Returns true if RESP_DELIM was the last character received.
    bool RN4870X_isDataComplete(void);
    
    //Get the number of remaining characters to read
    uint8_t RN4870RX_remaining(void);

    //Get the next character in the queue
    char RN4870RX_getCharacter(void);
    
    //Discards the buffer
    void RN4870RX_clearBuffer(void);
    
    //Waits for a message to be received and checks to see if it matches string.
    //Timeout is specified in milliseconds
    bool RN4870RX_waitForRX(uint16_t timeout, const char* compare);
    
#ifdef	__cplusplus
}
#endif

#endif	/* RN4870_RX_H */

