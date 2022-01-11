/**
  TCB0 Generated Driver API Header File
 
  @Company
    Microchip Technology Inc.
 
  @File Name
    tcb0.h
 
  @Summary
    This is the generated header file for the TCB0 driver
 
  @Description
    This header file provides APIs for driver for TCB0.
    Generation Information :
        Driver Version    :  1.1.0
    The generated drivers are tested against the following:
        Compiler          :  XC8 v2.20
        MPLAB             :  MPLAB X v5.40
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

#ifndef TCB0_H_INCLUDED
#define TCB0_H_INCLUDED

#include <stdint.h>
#include "../system/utils/compiler.h"
#include "./timer_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const struct TMR_INTERFACE TCB0_Interface;

void TCB0_Initialize(void);
void TCB0_Start(void);
void TCB0_Stop(void);
void TCB0_EnableCaptInterrupt(void);
void TCB0_DisableCaptInterrupt(void);
void TCB0_EnableOvfInterrupt(void);
void TCB0_DisableOvfInterrupt(void);
uint16_t TCB0_Read(void);
void TCB0_Write(uint16_t timerVal);
void TCB0_ClearCaptInterruptFlag(void);
bool TCB0_IsCaptInterruptEnabled(void);
void TCB0_ClearOvfInterruptFlag(void);
bool TCB0_IsOvfInterruptEnabled(void);

#ifdef __cplusplus
}
#endif

#endif /* TCB0_H_INCLUDED */