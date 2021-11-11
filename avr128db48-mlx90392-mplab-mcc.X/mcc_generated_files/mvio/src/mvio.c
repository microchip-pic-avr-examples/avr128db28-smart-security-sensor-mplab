/**
  MVIO Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    mvio.c

  @Summary
    This is the generated driver implementation file for the MVIO

  @Description
    This source file provides APIs for MVIO.
    Generation Information :
        Driver Version    :  1.0.0
    The generated drivers are tested against the following:
        Compiler          :  XC8 v2.20
        MPLAB             :  MPLABX v5.40
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

#include "../mvio.h"

static void (* onMVIOChange)(void);

/**
 * \brief Initialize mvio interface
 */
int8_t MVIO_Initialize(void) 
{
    //VDDIO2IE enabled; 
    MVIO.INTCTRL = 0x1;
    onMVIOChange = 0;

    return 0;
}

void MVIO_setCallback(void (*callback)())
{
    onMVIOChange = callback;
}

/**
 * \brief Check MVIO VDDIO2 Status
 *
 * \return The status to check if VDDIO2 voltage is within the acceptable range of operation
 * \retval false The VDDIO2 supply voltage is below the acceptable range of operation
 * \retval true The VDDIO2 supply voltage is within the acceptable range of operation
 */
bool MVIO_isOK(void) 
{
    if (MVIO.STATUS  & MVIO_VDDIO2S_bm) {
        return true;
    } else {
        return false;
    }
}

ISR(MVIO_MVIO_vect)
{
    if (onMVIOChange)
    {
        onMVIOChange();
    }
    
    //Clear MVIO Flag
    MVIO.INTFLAGS = 1;
}
