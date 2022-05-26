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

#ifndef GPIO_H
#define	GPIO_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <avr/io.h>
        
#define IO_VDIV_TurnOn() do { PORTA.OUTSET = PIN6_bm; } while (0)
#define IO_VDIV_TurnOff() do { PORTA.OUTCLR = PIN6_bm; } while (0)
    
#define IO_LED0R_TurnOn() do { PORTD.OUTSET = PIN3_bm; } while(0)
#define IO_LED0R_TurnOff() do { PORTD.OUTCLR = PIN3_bm; } while(0)
#define IO_LED0R_Toggle() do { PORTD.OUTTGL = PIN3_bm; } while(0)
#define IO_LED0R_GetValue() (VPORTD.OUT & PIN3_bm)

#define IO_LED0G_TurnOn() do { PORTD.OUTSET = PIN4_bm; } while(0)
#define IO_LED0G_TurnOff() do { PORTD.OUTCLR = PIN4_bm; } while(0)
#define IO_LED0G_Toggle() do { PORTD.OUTTGL = PIN4_bm; } while(0)
#define IO_LED0G_GetValue() (VPORTD.OUT & PIN4_bm)

#define IO_LED0B_TurnOn() do { PORTD.OUTSET = PIN5_bm; } while(0)
#define IO_LED0B_TurnOff() do { PORTD.OUTCLR = PIN5_bm; } while(0)
#define IO_LED0B_Toggle() do { PORTD.OUTTGL = PIN5_bm; } while(0)
#define IO_LED0B_GetValue() (VPORTD.OUT & PIN5_bm)

#define WAKE_GetValue() (VPORTA.IN & PIN0_bm)
#define WAKE_EnableIOC() do { PORTA.PIN0CTRL |= PORT_ISC_BOTHEDGES_gc; } while (0)
#define WAKE_DisableIOC() do { PORTA.PIN0CTRL &= ~(PORT_ISC_gm); } while (0)
#define WAKE_ClearFlag() do { VPORTA.INTFLAGS |= (PIN0_bm); } while (0)

#define BTLE_EnablePower() do { PORTD.OUTSET = PIN1_bm; } while (0)
#define BTLE_DisablePower() do { PORTD.OUTCLR = PIN1_bm; } while (0)
    
#define BTLE_AssertReset() do { PORTD.OUTCLR = PIN2_bm; } while (0)
#define BTLE_ReleaseReset() do { PORTD.OUTSET = PIN2_bm; } while (0)
    
#define BTLE_GetStatus() (VPORTA.IN & PIN1_bm)
    
    //Inits. General Purpose I/O
    void GPIO_init(void);

#ifdef	__cplusplus
}
#endif

#endif	/* GPIO_H */

