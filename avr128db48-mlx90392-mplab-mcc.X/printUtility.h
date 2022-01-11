#include <stdint.h>

#ifndef PRINTUTILITY_H
#define	PRINTUTILITY_H

#ifdef	__cplusplus
extern "C" {
#endif
    
//Disable String Messages for Data Plotting
//#define DISABLE_STRING_MESSAGES
        
#define PRINT_BUFFER_SIZE 255
    
    //Returns the Address of the character buffer
    char* getCharBufferUSB(void);
    
    //Returns the size of the char buffer
    uint8_t getCharBufferSizeUSB(void);
    
    //Prints the string on the UART
    void printBufferedStringUSB(void);
    
    //Prints a constant string to the UART
    void printConstantStringUSB(const char* text);
    
    //Returns the Address of the character buffer
    char* getCharBufferBLE(void);
    
    //Returns the size of the char buffer
    uint8_t getCharBufferSizeBLE(void);
    
    //Prints the string on the UART
    void printBufferedStringBLE(void);
    
    //Prints a constant string to the UART
    void printConstantStringBLE(const char* text);

    
#ifdef	__cplusplus
}
#endif

#endif	/* PRINTUTILITY_H */

