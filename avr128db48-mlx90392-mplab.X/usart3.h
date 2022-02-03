#ifndef USART3_H
#define	USART3_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h> 
    
    //Init Peripheral
    void USART3_init(void);
    
    //Init Peripheral I/O
    void USART3_initIO(void);

    //Enable/Disable TX 
    void USART3_enableTX(void);
    void USART3_disableTX(void);
    
    //Enable/Disable RX
    void USART3_enableRX(void);
    void USART3_disableRX(void);
    
    //Sets the callback function for Data RX
    void USART3_setRXCallback(void (*function)(char));
    
    //Sends a byte of data. Returns false if buffer cannot accept data
    bool USART3_sendByte(char c);
    
    //Returns true if the peripheral can accept a new byte
    bool USART3_canTransmit(void);
    
    //Returns true if the TX shifter is running
    bool USART3_isBusy(void);
    
    //Returns true if data is being shifted in
    bool USART3_isRXActive(void);

#ifdef	__cplusplus
}
#endif

#endif	/* USART_H */

