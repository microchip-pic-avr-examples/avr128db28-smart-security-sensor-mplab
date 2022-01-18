/**
  @Company
    Microchip Technology Inc.

  @Description
    This Source file provides APIs.
    Generation Information :
    Driver Version    :   1.0.0
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


#include "../system.h"

/**
 * Initializes MCU, drivers and middleware in the project
**/

int8_t SLPCTRL_Initialize();

void SYSTEM_Initialize(void)
{
    PIN_MANAGER_Initialize();
    CLOCK_Initialize();
    EVSYS_Initialize();
    MVIO_Initialize();
    NVM_Initialize();
    RTC_Initialize();
    SLPCTRL_Initialize();
    TCB0_Initialize();
    USART3_Initialize();
    VREF_Initialize();
    WDT_Initialize();
    CPUINT_Initialize();
}

/**
 * \brief Initialize SLPCTRL_Initialize interface
 */
int8_t SLPCTRL_Initialize()
{
    //SEN enabled; SMODE STDBY; 
    ccp_write_io((void*)&(SLPCTRL.CTRLA),0x3);
    
    //PMODE AUTO; 
    ccp_write_io((void*)&(SLPCTRL.VREGCTRL),0x0);
    

    return 0;
}


