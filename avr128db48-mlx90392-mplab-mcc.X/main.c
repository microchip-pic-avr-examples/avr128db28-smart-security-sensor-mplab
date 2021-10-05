/*
© [2021] Microchip Technology Inc. and its subsidiaries.

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
#include "mcc_generated_files/system/system.h"
#include "MLX90392.h"
#include "mcc_generated_files/timer/delay.h"

int main(void)
{
    SYSTEM_Initialize();
    
    //Enable TWI in Debug
    TWI0.DBGCTRL = 1;
    
    //Bug Fix for IO Assignment
    PORTMUX.TWIROUTEA = 0x2;
    
    //Leave enough time to ensure device is ready
    DELAY_milliseconds(1000);
    
    //Soft Reset the Sensor
    MLX90392_reset();
    
    //TODO: Verify +400 in self-test!!!
    if (!MLX90392_selfTest())
    {
        //If Sensor is Bad...
        
        //TODO: Change Behavior
        while (1) { 
            LED0_Toggle();
            DELAY_milliseconds(500);
        }
    }
    
    MLX90393_Result result;
    
    while(1)
    {
        //Get a single measurement
        MLX90392_getSingleMeasurement(&result);
        
        //Testing Code
        if (result.Z_Meas > 0)
        {
            LED0_SetHigh();
        }
        else
        {
            LED0_SetLow();
        }
        
        //Sleep
        asm("SLEEP");
    }    
}