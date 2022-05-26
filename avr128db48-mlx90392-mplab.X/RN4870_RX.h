/*
© [2022] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#ifndef RN4870_RX_H
#define	RN4870_RX_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
//Size of String Buffer for General Purpose RX
#define RN4870_RX_BUFFER_SIZE 255
    
//Size of Response Buffer (%TEXT%)
#define RN4870_RX_STATUS_BUFFER_SIZE 32
    
//Delimiters
#define RN4870_DELIM_USER '!'
#define RN4870_DELIM_STATUS '%'
#define RN4870_DELIM_RING_BUFFER '#'
    
    //Initializes the RX Engine for the RN4870
    void RN4870RX_init(void);
    
    //Insert a character into the buffer
    void RN4870RX_loadCharacter(char input);
    
    //Returns true if status / user commands are empty
    bool RN4870RX_isEmpty(void);
    
    //Returns true if a status message (%TEXT%) was received
    bool RN4870RX_isStatusRX(void);
    
    //Returns true if a user command (!TEXT!) was received
    bool RN4870RX_isUserRX(void);
    
    //Clears status flag
    void RN4870RX_clearStatusRX(void);
    
    //Returns true if status matches COMP string
    bool RN4870RX_find(const char* comp);
        
    //Advances to the next status / command in the buffer, if available.
    void RN4870RX_advanceMessage(void);
    
    //Fills a buffer with a copy of the current message
    void RN4870RX_copyMessage(char* buffer, uint8_t size);
    
    //Fills a buffer with a copy of the parameter of the command.
    //Returns false if no parameter is present
    bool RN4870RX_copyMessageParameter(char* buffer, uint8_t size);
    
    //Returns true if a deliminater was received.
    bool RN4870RX_isResponseComplete(void);
    
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

