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

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>
#include <stdbool.h>

#include "GPIO.h"

#include "windowAlarm.h"
#include "tempMonitor.h"

#include "RN4870.h"
#include "RN4870_RX.h"

#include "usart2.h"
#include "usart0.h"

#include "TWI0_host.h"

#include "MVIO.h"
#include "system.h"
#include "TCB0_oneShot.h"
#include "RTC.h"
#include "printUtility.h"
#include "demo.h"

int main(void)
{
    //Init System / Core Peripherals
    System_initDevice();
    
    //Init Peripherals and IO
    System_initPeripherals();
        
    //Attach Callback Function for USART0
    USART0_setRXCallback(&RN4870RX_loadCharacter);
    
    //Configure Callback for User Commands
    RN4870_setUserEventHandler(&DEMO_handleUserCommands);
    
    //Setup ISR Callback for RTC
    RTC_setCMPCallback(&tempMonitor_requestConversion);
                
    //This boolean is used to determine if reset to defaults is required
    bool safeStart = WAKE_GetValue();
    
    //Init User Settings
    DEMO_init(safeStart);
    
    //Init State Machines
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
                                                
        //Check for Events
        RN4870_processEvents();
        
        //Run the magnetometer state machine
        windowAlarm_FSM();
        
        //If the calibration for the window alarm is OK
        if (windowAlarm_isCalGood())
        {
            //Run the thermometer state machine
            tempMonitor_FSM();
        }
        
        if (RTC_isOVFTriggered())
        {
            RTC_clearOVFTrigger();

            if (RN4870_isReady())
            {
                if (windowAlarm_isCalGood())
                {
                    //If not calibrating...
                    windowAlarm_printResults();
                    tempMonitor_printResults();
                }
            }
        }

        if (RN4870_canSleep())
        {
            //If the module is powered off, then sleep
            asm("SLEEP");
        }
        else
        {
            //Can't enter sleep, but we should wait for the next PIT trigger
            
            //Wait for PIT Trigger
            RTC_clearPITTriggered();
            while (!RTC_isPITTriggered()) { ; }
        }
    }    
}