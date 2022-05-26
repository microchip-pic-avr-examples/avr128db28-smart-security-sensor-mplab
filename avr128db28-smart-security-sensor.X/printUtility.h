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
    char* USB_getCharBuffer(void);
    
    //Returns the size of the char buffer
    uint8_t USB_getCharBufferSize(void);
    
    //Prints the string on the UART
    void USB_sendBufferedString(void);
    
    //Prints the literal text to the string
    void USB_sendStringRaw(const char* text);
    
    //Prints a constant string to the UART, then appends \r\n
    void USB_sendStringWithEndline(const char* text);
    
    //Returns the Address of the character buffer
    char* BLE_getCharBuffer(void);
    
    //Returns the size of the char buffer
    uint8_t BLE_getCharBufferSize(void);
    
    //Prints the string on the UART
    void BLE_sendBufferedString(void);
    
    
    //Prints a constant string to the UART
    void BLE_sendStringRaw(const char* text);
    
    //Prints a constant string to the UART, then appends \r\n
    void BLE_sendStringWithEndline(const char* text);

    
    //Prints a constant string to the UART, then appends DELIM
    void BLE_printCommandString(const char* text, const char delim);
    
#ifdef	__cplusplus
}
#endif

#endif	/* PRINTUTILITY_H */

