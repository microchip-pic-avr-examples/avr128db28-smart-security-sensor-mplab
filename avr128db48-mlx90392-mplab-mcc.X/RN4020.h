#ifndef RN4020_H
#define	RN4020_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
    
#define DEFAULT_BUFFER_SIZE 16
    
    //Inits the RN4020 BLE Module
    void RN4020_initDevice(void);

    //Prints a string to the UART
    void RN4020_sendUART(const char* str, uint8_t len);

    //Send a NULL terminated string to the UART
    void RN4020_sendStringUART(const char* str);

#ifdef	__cplusplus
}
#endif

#endif	/* RN4020_H */

