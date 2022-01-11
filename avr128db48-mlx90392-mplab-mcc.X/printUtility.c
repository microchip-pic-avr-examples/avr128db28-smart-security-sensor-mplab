#include <xc.h>
#include <stdint.h>

#include "printUtility.h"
#include "mcc_generated_files/system/system.h"

static char bufferUSB[PRINT_BUFFER_SIZE];
static volatile char bufferBLE[PRINT_BUFFER_SIZE];

//Returns the Address of the character buffer
char* getCharBufferUSB(void)
{
    return &bufferUSB[0];
}

//Returns the size of the char buffer
uint8_t getCharBufferSizeUSB(void)
{
    return PRINT_BUFFER_SIZE;
}

//Prints the string on the UART
void printBufferedStringUSB(void)
{   
    if (bufferUSB[0] == '\0')
        return;
    
    uint8_t index = 0;
    
    //Clear USART Done Flag
    USART3.STATUS |= USART_TXCIF_bm;
    
    while ((bufferUSB[index] != '\0') && (index < PRINT_BUFFER_SIZE))
    {
        while (!USART3_IsTxReady());
        
        USART3_Write(bufferUSB[index]);
        index++;
    }
    
    //Wait for completion of UART
    while (!USART3_IsTxReady());
    while (USART3_IsTxBusy());
}

//Prints a constant string to the UART
void printConstantStringUSB(const char* text)
{
#ifdef DISABLE_STRING_MESSAGES
    return;
#else    
    
    if (text[0] == '\0')
        return;
    
    uint8_t index = 0;
    
    //Clear USART Done Flag
    USART3.STATUS |= USART_TXCIF_bm;
    
    while ((text[index] != '\0') && (index < 255))
    {
        while (!USART3_IsTxReady());
        
        USART3_Write(text[index]);
        index++;
    }
    
    //Wait for completion of UART
    while (!USART3_IsTxReady());
    while (USART3_IsTxBusy());
#endif
}

//Returns the Address of the character buffer
char* getCharBufferBLE(void)
{
    return &bufferBLE[0];
}

//Returns the size of the char buffer
uint8_t getCharBufferSizeBLE(void)
{
    return PRINT_BUFFER_SIZE;
}

//Prints the string on the UART
void printBufferedStringBLE(void)
{   
    if (bufferBLE[0] == '\0')
        return;
    
    uint8_t index = 0;
    
    //Clear USART Done Flag
    USART2.STATUS |= USART_TXCIF_bm;
    
    while ((bufferBLE[index] != '\0') && (index < PRINT_BUFFER_SIZE))
    {
        while (!USART2_IsTxReady());
        
        USART2_Write(bufferBLE[index]);
        index++;
    }
    
    //Wait for completion of UART
    while (!USART2_IsTxReady());
    while (USART2_IsTxBusy());
}

//Prints a constant string to the UART
void printConstantStringBLE(const char* text)
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
        while (!USART2_IsTxReady());
        
        USART2_Write(text[index]);
        index++;
    }
    
    //Wait for completion of UART
    while (!USART2_IsTxReady());
    while (USART2_IsTxBusy());
#endif
}