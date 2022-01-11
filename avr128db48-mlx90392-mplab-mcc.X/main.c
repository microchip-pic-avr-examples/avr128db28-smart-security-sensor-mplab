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

int main(void)
{
    SYSTEM_Initialize();
      
    //Configure RN4870
    RN4870_init();
    
    //Debug RESET Conditions
//    sprintf(getCharBufferUSB(), "RSTCTRL = 0x%x\r\n", RSTCTRL.RSTFR);
//    printBufferedStringUSB();
//    
//    sprintf(getCharBufferUSB(), "FUSE.WDT = 0x%x\r\n", FUSE.WDTCFG);
//    printBufferedStringUSB();
//    
//    sprintf(getCharBufferUSB(), "WDT.CTRLA = 0x%x\r\nWDT.STATUS = 0x%x\r\n", WDT.CTRLA, WDT.STATUS);
//    printBufferedStringUSB();
    
    //Clear reset 
    RSTCTRL.RSTFR = 0xFF;
    
    //Enable TWI in Debug
    TWI0.DBGCTRL = 1;
    TWI1.DBGCTRL = 1;
    
    //Enable USART in Debug
    USART3.DBGCTRL = 1;
    
    //Enable USART Debug
    USART3.DBGCTRL = 1;
    
    //Bug Fix for IO Assignment
    PORTMUX.TWIROUTEA = 0x2;
    
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