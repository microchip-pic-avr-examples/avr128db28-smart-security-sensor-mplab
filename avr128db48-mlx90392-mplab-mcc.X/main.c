/*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

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
#include <xc.h>

#include "mcc_generated_files/system/system.h"
#include "MLX90392.h"
#include "MLX90632.h"
#include "mcc_generated_files/timer/delay.h"
#include "windowAlarm.h"
#include "tempMonitor.h"
#include "printUtility.h"

#include "RN4870.h"
#include "RN4870_RX.h"

#include "usart3.h"
#include "usart2.h"

#include "TWI0_host.h"
#include "TWI1_host.h"

#include "MVIO.h"

int main(void)
{
    SYSTEM_Initialize();
    
    //Init MVIO
    MVIO_init();
    
    //Setup Callback function
    MVIO_setCallback(&_windowAlarm_onMVIOChange);
    
    //Configure TWI0 (MVIO) for Magnetometer
    TWI0_initHost();
    TWI0_initPins();
    
    //Configure TWI1 for Thermometer
    TWI1_initHost();
    TWI1_initPins();
    
    //Configure USART 2 for BLE
    USART2_init();
    USART2_initIO();
    
    //Configure USART 3 for USB
    USART3_init();
    USART3_initIO();
    
    //Attach Callback Function for USART2
    USART2_setRXCallback(&RN4870RX_loadCharacter);
    
    //Enable USART for BLE
    USART2_enableRX();
    USART2_enableTX();
    
    //Enable USART for USB (TX Only)
    USART3_enableTX();
    
    //Setup ISR Callback for RTC
    RTC_SetOVFIsrCallback(&tempMonitor_requestConversion);
    
    //Setup MVIO ISR
    MVIO_setCallback(&_windowAlarm_onMVIOChange);
            
    //This boolean is used to determine if reset to defaults is required
    bool safeStart = SW0_GetValue();
            
    windowAlarm_init(safeStart);
    tempMonitor_init(safeStart);
    
    //Start Interrupts
    sei();
    
    //Configure RN4870
    RN4870_init();
    
    while(1)
    {        
        //Clear the Watchdog Timer
        asm("WDR");
                                
        //Run the magnetometer state machine
        windowAlarm_FSM();
        
        //Run the thermometer state machine
        tempMonitor_FSM();
                        
        asm("SLEEP");
    }    
}