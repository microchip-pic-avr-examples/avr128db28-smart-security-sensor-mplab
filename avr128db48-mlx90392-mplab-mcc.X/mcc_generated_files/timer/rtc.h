/**
  @Company
    Microchip Technology Inc.

  @Description
    This Source file provides APIs.
    Generation Information :
    Driver Version    :   2.0.0
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


#ifndef RTCDRIVER_H
#define RTCDRIVER_H

#include "../system/utils/compiler.h"
#include <stdint.h>
#include <stdbool.h>

/** Datatype used to hold the number of ticks until a timer expires */

typedef void (*RTC_cb_t)(void);
void RTC_SetOVFIsrCallback(RTC_cb_t cb);
void RTC_SetCMPIsrCallback(RTC_cb_t cb);
void RTC_SetPITIsrCallback(RTC_cb_t cb);
int8_t RTC_Initialize(void);
void RTC_Start(void);
void RTC_Stop(void);
void RTC_WriteCounter(uint16_t timerVal);
void RTC_WritePeroid(uint16_t timerVal);
uint16_t RTC_ReadCounter(void);
uint16_t RTC_ReadPeriod(void);
void RTC_EnableCMPInterrupt(void);
void RTC_DisableCMPInterrupt(void);
void RTC_EnableOVFInterrupt(void);
void RTC_DisableOVFInterrupt(void);
void RTC_EnablePITInterrupt(void);
void RTC_DisablePITInterrupt(void);
void RTC_ClearOVFInterruptFlag(void);
bool RTC_IsOVFInterruptEnabled(void);


#endif /* RTCDRIVER_H */

/** @}*/