#ifndef GPIO_H
#define	GPIO_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <avr/io.h>
        
#define LED0R_TurnOn() do { PORTD.OUTSET = PIN3_bm; } while(0)
#define LED0R_TurnOff() do { PORTD.OUTCLR = PIN3_bm; } while(0)
#define LED0R_Toggle() do { PORTD.OUTTGL = PIN3_bm; } while(0)

#define LED0G_TurnOn() do { PORTD.OUTSET = PIN4_bm; } while(0)
#define LED0G_TurnOff() do { PORTD.OUTCLR = PIN4_bm; } while(0)
#define LED0G_Toggle() do { PORTD.OUTTGL = PIN4_bm; } while(0)

#define LED0B_TurnOn() do { PORTD.OUTSET = PIN5_bm; } while(0)
#define LED0B_TurnOff() do { PORTD.OUTCLR = PIN5_bm; } while(0)
#define LED0B_Toggle() do { PORTD.OUTTGL = PIN5_bm; } while(0)

#define WAKE_GetValue() (VPORTA.IN & PIN0_bm)
#define WAKE_EnableIOC() do { PORTA.PIN0CTRL |= PORT_ISC_BOTHEDGES_gc; } while (0)
#define WAKE_DisableIOC() do { PORTA.PIN0CTRL &= ~(PORT_ISC_gm); } while (0)
#define WAKE_ClearFlag() do { VPORTA.INTFLAGS |= (PIN0_bm); } while (0)

#define BTLE_EnablePower() do { PORTD.OUTCLR = PIN1_bm; } while (0)
#define BTLE_DisablePower() do { PORTD.OUTSET = PIN1_bm; } while (0)
    
#define BTLE_AssertReset() do { PORTD.OUTCLR = PIN2_bm; } while (0)
#define BTLE_ReleaseReset() do { PORTD.OUTSET = PIN2_bm; } while (0)
    
#define BTLE_GetStatus() (VPORTA.IN & PIN1_bm)
    
    //Inits. General Purpose I/O
    void GPIO_init(void);

#ifdef	__cplusplus
}
#endif

#endif	/* GPIO_H */

