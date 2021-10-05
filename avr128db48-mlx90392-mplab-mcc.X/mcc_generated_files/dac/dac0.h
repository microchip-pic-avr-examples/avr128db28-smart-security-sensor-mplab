/**
 * DAC0 Generated Driver API Header File
 * 
 * @file dac0.h
 * 
 * @defgroup  dac0 DAC0
 * 
 * @brief This is the generated header file for the DAC0 driver
 *
 * @version DAC0 Driver Version 1.0.0
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


#ifndef DAC0_H_INCLUDED
#define DAC0_H_INCLUDED

#include "../system/utils/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Datatype for the resolution of the DAC */
typedef uint16_t dac_resolution_t;

/**
 * @ingroup dac0
 * @brief  This routine initializes the DAC0 and must be called only once, before any other DAC0 routine is called.
 * If module is configured to disabled state, the clock to the DAC is disabled
 * if this is supported by the device's clock system.
 * @param void
 * @return Initialization status.
 * @retval 0 the DAC init was successful
 * @retval 1 the DAC init was not successful
 */
int8_t DAC0_Initialize(void);

/**
 * @ingroup dac0
 * @brief  This routine enables the DAC0
 * @param void
 * @return void
 */
void DAC0_Enable(void);

/**
 * @ingroup dac0
 * @brief  This routine disables the DAC0
 * @param void
 * @return void
 */
void DAC0_Disable(void);

/**
 * @ingroup dac0
 * @brief  This routine starts a conversion on DAC0
 * @param value - The digital value to be converted to analog voltage by the DAC
 * @return void
 */
 void DAC0_SetOutput(dac_resolution_t value);

/**
 * @ingroup dac0
 * @brief  This routine returns the number of bits in the DAC0 resolution
 * @param void
 * @return The number of bits in the resolution
 */
uint8_t DAC0_GetResolution(void);

#ifdef __cplusplus
}
#endif

#endif /* DAC0_H_INCLUDED */