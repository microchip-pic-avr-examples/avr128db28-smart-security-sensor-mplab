#include "RN4020.h"
#include "mcc_generated_files/system/pins.h"
#include "mcc_generated_files/uart/usart2.h"
#include "mcc_generated_files/timer/delay.h"
#include <xc.h>

#include <stdint.h>
#include <stdbool.h>

#define RN4020_ENTER_CMD() do { RN4020_CMD_SetHigh(); } while (0)
#define RN4020_EXIT_CMD() do { RN4020_CMD_SetLow(); } while (0)

#define RN4020_WAKE() do { RN4020_WAKE_SetHigh(); } while (0)
#define RN4020_SLEEP() do {RN4020_WAKE_SetLow(); } while (0)

static volatile char buffer[DEFAULT_BUFFER_SIZE];

void RN4020_initDevice(void)
{
    RN4020_WAKE();
    RN4020_ENTER_CMD();
        
    //DELAY_milliseconds(4000);
    
    RN4020_sendStringUART("S-,MLX90392 Demo\r\n");
    RN4020_sendStringUART("R\r\n");
    RN4020_SLEEP();
}

void RN4020_sendUART(const char* str, uint8_t len)
{
    uint8_t index = 0;
    
    while (index < len)
    {
        //Wait for Ready
        while (!USART2_IsTxReady());
        
        //Write the Byte
        USART2_Write(str[index]);
        
        index++;
    }
    
    while (!USART2_IsTxDone());
        USART2.STATUS = USART_TXCIF_bm;
}

void RN4020_sendStringUART(const char* str)
{
    uint8_t index = 0;
    
    while (str[index] != '\0')
    {
        
        while (!USART2_IsTxReady());
        
        //Write the Byte
        USART2_Write(str[index]);
        
        index++;
    }
    
    while (!USART2_IsTxDone());
        USART2.STATUS = USART_TXCIF_bm;
}