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
#include "MLX90632.h"
#include "mcc_generated_files/timer/delay.h"
#include "RN4020.h"

void sensorFailure(void)
{
    while (1)
    {
        LED0_Toggle();
        DELAY_milliseconds(500);
    }
}

int main(void)
{
    SYSTEM_Initialize();
        
    //Enable TWI in Debug
    TWI0.DBGCTRL = 1;
    TWI1.DBGCTRL = 1;
    
    //Bug Fix for IO Assignment
    PORTMUX.TWIROUTEA = 0x2;
    
    DELAY_milliseconds(10);
    
    //Init RN4020 BLE Module
    //RN4020_initDevice();
    
    MLX90632_initDevice();
        
    uint8_t tries = 10;
    
    //Init Magnetometer
//    do
//    {
//        //Soft Reset the Sensor
//        tries--;
//        
//        if (tries == 0)
//        {
//            //Unable to reset sensor
//            
//            sensorFailure();
//        }
//        
//    } while (MLX90392_reset());
    
    
    
    //Only perform Magnetometer self test if sensor is -010 version 
//    if (!MLX90392_selfTest())
//    {
//        //If Sensor is Bad...
//        
//        sensorFailure();
//    }
    
    MLX90392_Result result;
    bool success;
    volatile float temp;
    
    sei();
    
    while(1)
    {
        //Get a single measurement
        //MLX90392_getSingleMeasurement(&result);
        
        temp = MLX90632_computeTemperature();
        
        //Sleep
        LED0_Toggle();
        asm("SLEEP");
    }    
}