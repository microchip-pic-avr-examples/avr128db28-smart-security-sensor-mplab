#ifndef USART0_H
#define	USART0_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h> 
    
    //Init Peripheral
    void USART0_init(void);
        
    //Init Peripheral I/O
    void USART0_initIO(void);
    
    //Enable/Disable TX 
    void USART0_enableTX(void);
    void USART0_disableTX(void);
    
    //Enable/Disable RX
    void USART0_enableRX(void);
    void USART0_disableRX(void);
    
    //Sets the callback function for Data RX
    void USART0_setRXCallback(void (*function)(char));
    
    //Sends a byte of data. Returns false if buffer cannot accept data
    bool USART0_sendByte(char c);
    
    //Returns true if the peripheral can accept a new byte
    bool USART0_canTransmit(void);
    
    //Returns true if the TX shifter is running
    bool USART0_isBusy(void);
    
    //Returns true if data is being shifted in
    bool USART0_isRXActive(void);

#ifdef	__cplusplus
}
#endif

#endif	/* USART0_H */

