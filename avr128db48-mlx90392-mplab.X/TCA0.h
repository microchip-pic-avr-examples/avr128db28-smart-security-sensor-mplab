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

#ifndef TCA0_H
#define	TCA0_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define TCA_SPLIT_PERIOD 30
    
#define TCA_LEDR_ON_TIME 3
#define TCA_LEDG_ON_TIME 3
#define TCA_LEDB_ON_TIME 3
    
#include <stdbool.h>
    
    //Init the TCA Peripheral
    void TCA0_init(void);
    
    //Init TCA IO
    void TCA0_initIO(void);
    
    //Enable CMP Outputs
    void TCA0_enableHCMP0(void);
    void TCA0_enableHCMP1(void);
    void TCA0_enableHCMP2(void);
    
#define TCA0_enableLEDR TCA0_enableHCMP0
#define TCA0_enableLEDG TCA0_enableHCMP1
#define TCA0_enableLEDB TCA0_enableHCMP2

    //Disable CMP Outputs
    void TCA0_disableHCMP0(void);
    void TCA0_disableHCMP1(void);
    void TCA0_disableHCMP2(void);
    
#define TCA0_disableLEDR TCA0_disableHCMP0
#define TCA0_disableLEDG TCA0_disableHCMP1
#define TCA0_disableLEDB TCA0_disableHCMP2

    //Returns true if the HCMP is enabled (ie: PWM is ON)
    bool TCA0_getHCMP0EN(void);
    bool TCA0_getHCMP1EN(void);
    bool TCA0_getHCMP2EN(void);
    
#define TCA0_getLEDR TCA0_getHCMP0EN
#define TCA0_getLEDG TCA0_getHCMP1EN
#define TCA0_getLEDB TCA0_getHCMP2EN
    
#ifdef	__cplusplus
}
#endif

#endif	/* TCA0_H */

