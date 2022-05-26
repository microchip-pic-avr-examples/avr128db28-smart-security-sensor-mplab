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
#include "TCA0.h"
#include "LEDcontrol.h"
#include "Bluetooth_Timeout_Timer.h"
#include "Welcome_Timer.h"

//Prints results when not in low-power mode
void normalPrint(void)
{   
    //Update LED State
    DEMO_setAlarmLEDs();
    
    //Check to see if the welcome message should be sent
    if (welcomeTimer_hasTriggered())
    {
        RN4870_sendStringToUser("Welcome to the Microchip Security Sensor Demo!\r\n");
               
        //Disable Welcome Timer
        welcomeTimer_stop();
        RTC_clearOVFTrigger();
        
        //If ready to print results
        if (windowAlarm_getResultStatus())
        {
            windowAlarm_printResults();
        }
        
        if (tempMonitor_getResultStatus())
        {
            tempMonitor_printResults();
        }
        
        //Reset the RTC to remove sampling glitch
        RTC_reset();
        return;
    }
    else if ((welcomeTimer_isRunning()) || (!RN4870_isConnected()))
    {
        //If waiting for welcome or Bluetooth is not connected...
        return;
    }
    
    if (windowAlarm_getResultStatus())
    {
        windowAlarm_printResults();
    }
    else if (RTC_isOVFTriggered())
    {
        RTC_clearOVFTrigger();
        
        windowAlarm_printResults();
        if (windowAlarm_isCalGood())
        {
            //If not calibrating...
            tempMonitor_printResults();
        }
    }
}

//Updates display in low power mode
//LEDs are on for ~1s 
void lowPowerLEDPrint(void)
{
    if (RTC_isOVFTriggered())
    {
        RTC_clearOVFTrigger();
        
        //Update LED states
        DEMO_setAlarmLEDs();
    }
    else
    {
        //Shutoff LEDs
        LED_turnOffGreen();
        LED_turnOffRed();
    }
}

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
    
    //Set Starting Time of BLE Auto-Off Timer
    BLE_SW_Timer_setCurrentTime();
    
    while(1)
    {        
        //Clear the Watchdog Timer
        asm("WDR");
                                                
        //Check for Events
        RN4870_processEvents();
                
        //If in sleep, always run the FSM loops
        //If not, then only run them once per PIT trigger
        if ((RN4870_canSleep()) || ((!RN4870_canSleep()) && (RTC_isPITTriggered())))
        {
            RTC_clearPITTriggered();
            
            //Run the magnetometer state machine
            windowAlarm_FSM();

            //If the calibration for the window alarm is OK AND
            //Not in sleep mode (or Run in Sleep is set)
            if ((windowAlarm_isCalGood()) && ((!RN4870_canSleep()) || tempMonitor_getRunInSleep()))
            {
                //Run the thermometer state machine
                tempMonitor_FSM();
            }
        }
        
        //Magnetometer Alarm

        if (RN4870_canSleep())
        {
            //If we can sleep (ie: BLE is OFF)...
            
            //Update LED states, if applicable
            lowPowerLEDPrint();
            
            if (windowAlarm_isAlarmOK() && tempMonitor_isTempNormal())
            {
                //Alarm OK - continue sleep
                asm("SLEEP");
            }
            else
            {
                //Alarm tripped - wake-up
                RN4870_powerUp();                
            }         
        }
        else
        {
            //Can't enter sleep, print normally
            normalPrint();
            
            //Add RTC Changes to SW Timers
            BLE_SW_Timer_addTime();
            welcomeTimer_addTime();
            
            //If Timer triggered
            if ((BLE_SW_Timer_hasTriggered()) && (windowAlarm_isAlarmOK()) &&
                    (windowAlarm_isCalGood()) && (!RN4870_isConnected()))
            {
                USB_sendStringWithEndline("Bluetooth Idle timeout has occurred. Switching to low power mode.");
                RN4870_powerDown();
            }
        }
    }    
}