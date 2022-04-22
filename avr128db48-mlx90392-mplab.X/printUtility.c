#include <avr/io.h>
#include <stdint.h>

#include "printUtility.h"

#include "usart0.h"
#include "usart2.h"

static char bufferUSB[PRINT_BUFFER_SIZE];
static char bufferBLE[PRINT_BUFFER_SIZE];

//Returns the Address of the character buffer
char* USB_getCharBuffer(void)
{
    return &bufferUSB[0];
}

//Returns the size of the char buffer
uint8_t USB_getCharBufferSize(void)
{
    return PRINT_BUFFER_SIZE;
}

//Prints the string on the UART
void USB_sendBufferedString(void)
{   
    if (bufferUSB[0] == '\0')
        return;
    
    uint8_t index = 0;
    
    //Clear USART Done Flag
    USART2.STATUS |= USART_TXCIF_bm;
    
    while ((bufferUSB[index] != '\0') && (index < PRINT_BUFFER_SIZE))
    {
        while (!USART2_canTransmit());
        
        USART2_sendByte(bufferUSB[index]);
        index++;
    }
    
    //Wait for completion of UART
    while (!USART2_canTransmit());
    while (USART2_isBusy());
}

//Prints a constant string to the UART
void USB_sendStringRaw(const char* text)
{
#ifdef DISABLE_STRING_MESSAGES
    return;
#else    
    if (text[0] == '\0')
        return;
    
    uint8_t index = 0;
    
    //Clear USART Done Flag
    USART2.STATUS |= USART_TXCIF_bm;
    
    while ((text[index] != '\0') && (index < 255))
    {
        while (!USART2_canTransmit());
        
        USART2_sendByte(text[index]);
        index++;
    }
    
    //Wait for completion of UART
    while (!USART2_canTransmit());
    while (USART2_isBusy());
#endif
}


//Prints a constant string to the UART
void USB_sendStringWithEndline(const char* text)
{
#ifdef DISABLE_STRING_MESSAGES
    return;
#else    
    if (text[0] == '\0')
        return;

    USB_sendStringRaw(text);
    USB_sendStringRaw("\r\n");
    
#endif
}

//Returns the Address of the character buffer
char* BLE_getCharBuffer(void)
{
    return &bufferBLE[0];
}

//Returns the size of the char buffer
uint8_t BLE_getCharBufferSize(void)
{
    return PRINT_BUFFER_SIZE;
}

//Prints the string on the UART
void BLE_sendBufferedString(void)
{   
    if (bufferBLE[0] == '\0')
        return;
    
    uint8_t index = 0;
    
    //Clear USART Done Flag
    USART0.STATUS |= USART_TXCIF_bm;
    
    while ((bufferBLE[index] != '\0') && (index < PRINT_BUFFER_SIZE))
    {
        while (!USART0_canTransmit());
        
        USART0_sendByte(bufferBLE[index]);
        index++;
    }
    
    //Wait for completion of UART
    while (!USART0_canTransmit());
    while (USART0_isBusy());
}

//Prints a constant string to the UART
void BLE_sendStringRaw(const char* text)
{   
    if (text[0] == '\0')
        return;
    
    uint8_t index = 0;
    
    //Clear USART Done Flag
    USART0.STATUS |= USART_TXCIF_bm;
    
    while ((text[index] != '\0') && (index < 255))
    {
        while (!USART0_canTransmit());
        
        USART0_sendByte(text[index]);
        index++;
    }
    
    //Wait for completion of UART
    while (!USART0_canTransmit());
    while (USART0_isBusy());
}

//Prints a constant string to the UART, then appends \r\n
void BLE_sendStringWithEndline(const char* text)
{
    BLE_sendStringRaw(text);
    BLE_sendStringRaw("\r\n");
}

void BLE_printCommandString(const char* text, const char delim)
{
    if (text[0] == '\0')
        return;
    
    uint8_t index = 0;
    
    //Clear USART Done Flag
    USART0.STATUS |= USART_TXCIF_bm;
    
    while ((text[index] != '\0') && (index < 255))
    {
        while (!USART0_canTransmit());
        
        USART0_sendByte(text[index]);
        index++;
    }
    
    //Send Deliminator
    while (!USART0_canTransmit());
    USART0_sendByte(delim);
    
    //Wait for completion of UART
    while (!USART0_canTransmit());
    while (USART0_isBusy());

}