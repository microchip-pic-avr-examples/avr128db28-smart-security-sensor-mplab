#ifndef USART2_H
#define	USART2_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h> 
    
    //Init Peripheral
    void USART2_init(void);
    
    //Init Peripheral I/O
    void USART2_initIO(void);

    //Enable/Disable TX 
    void USART2_enableTX(void);
    void USART2_disableTX(void);
    
    //Enable/Disable RX
    void USART2_enableRX(void);
    void USART2_disableRX(void);
    
    //Sets the callback function for Data RX
    void USART2_setRXCallback(void (*function)(char));
    
    //Sends a byte of data. Returns false if buffer cannot accept data
    bool USART2_sendByte(char c);
    
    //Returns true if the peripheral can accept a new byte
    bool USART2_canTransmit(void);
    
    //Returns true if the TX shifter is running
    bool USART2_isBusy(void);
    
    //Returns true if data is being shifted in
    bool USART2_isRXActive(void);

#ifdef	__cplusplus
}
#endif

#endif	/* USART2_H */

