/**
  @Company
    Microchip Technology Inc.

  @Description
    This Source file provides APIs.
    Generation Information :
    Driver Version    :   1.0.0
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

#include "../opamp.h"

void OPAMP_Initialize(void)
{
    OPAMP_DisableSystem();
    
    //TIMEBASE 3; 
    OPAMP.TIMEBASE = OPAMP_TIMEBASE_VALUE << OPAMP_TIMEBASE_gp;
    
    //DBGRUN enabled; 
    OPAMP.DBGCTRL = 0x1;
    
    //IRSEL REDUCED; 
    OPAMP.PWRCTRL = 0x1;
    
    //OP0 Configuration : Voltage Follower
    //OP0 Gain : 1
    
    //MUXNEG OPn output (unity gain); MUXPOS DAC output; 
    OPAMP.OP0INMUX = 0x22;
    
    //MUXBOT Multiplexer off; MUXTOP Multiplexer off; MUXWIP R1 = 15R, R2 = 1R, R2/R1 = 0.07; 
    OPAMP.OP0RESMUX = 0x0;
    
    //ALWAYSON enabled; EVENTEN disabled; OUTMODE Output Driver in Normal Mode; RUNSTBY disabled; 
    OPAMP.OP0CTRLA = 0x5;
    
    //SETTLE 127; 
    OPAMP.OP0SETTLE = 0x7F;
    
    //OP1 Configuration : Connected Directly to Pins
    
    //MUXNEG Negative input pin for OPn; MUXPOS Positive input pin for OPn; 
    OPAMP.OP1INMUX = 0x0;
    
    //MUXBOT Multiplexer off; MUXTOP Multiplexer off; MUXWIP R1 = 15R, R2 = 1R, R2/R1 = 0.07; 
    OPAMP.OP1RESMUX = 0x0;
    
    //ALWAYSON disabled; EVENTEN enabled; OUTMODE Output Driver Off; RUNSTBY disabled; 
    OPAMP.OP1CTRLA = 0x2;
    
    //SETTLE 127; 
    OPAMP.OP1SETTLE = 0x7F;
    
    //OP2 Configuration : Connected Directly to Pins
    
    //MUXNEG Negative input pin for OPn; MUXPOS Positive input pin for OPn; 
    OPAMP.OP2INMUX = 0x0;
    
    //MUXBOT Multiplexer off; MUXTOP Multiplexer off; MUXWIP R1 = 15R, R2 = 1R, R2/R1 = 0.07; 
    OPAMP.OP2RESMUX = 0x0;
    
    //ALWAYSON disabled; EVENTEN enabled; OUTMODE Output Driver Off; RUNSTBY disabled; 
    OPAMP.OP2CTRLA = 0x2;
    
    //SETTLE 127; 
    OPAMP.OP2SETTLE = 0x7F;
    
    OPAMP_EnableSystem();
}

void OPAMP_EnableSystem(void)
{
    OPAMP.CTRLA |= OPAMP_ENABLE_bm;
}

void OPAMP_DisableSystem(void)
{
    OPAMP.CTRLA &= ~OPAMP_ENABLE_bm;
}

//OP0 Voltage Follower APIs

void OPAMP_SetOP0PositiveInMux(OPAMP_OP0INMUX_MUXPOS_t value)
{
    OPAMP.OP0INMUX = (OPAMP.OP0INMUX & ~OPAMP_MUXPOS_gm) | value;
}

void OPAMP_SetOP0SettleTime(uint8_t settleTime)
{
    if (settleTime > 127) {
        settleTime = 127;
    }
    OPAMP.OP0SETTLE = settleTime;
}

bool OPAMP_IsOP0Settled(void)
{
    return OPAMP.OP0STATUS > 0;
}

void OPAMP_SetOP0OffsetCalibration(uint8_t calValue)
{
    OPAMP.OP0CAL = calValue;
}
