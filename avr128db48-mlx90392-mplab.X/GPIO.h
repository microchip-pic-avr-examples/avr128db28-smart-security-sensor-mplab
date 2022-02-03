#ifndef GPIO_H
#define	GPIO_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <avr/io.h>
    
#define SW0_GetValue() (VPORTB.IN & PIN2_bm)

#define LED0_SetHigh() do { PORTB.OUTSET = PIN3_bm; } while(0)
#define LED0_SetLow() do { PORTB.OUTCLR = PIN3_bm; } while(0)
#define LED0_Toggle() do { PORTB.OUTTGL = PIN3_bm; } while(0)

#define RN4870_MODE_SetHigh() do { PORTD.OUTSET = PIN0_bm; } while(0)
#define RN4870_MODE_SetLow() do { PORTD.OUTCLR = PIN0_bm; } while(0)
#define RN4870_MODE_Toggle() do { PORTD.OUTTGL = PIN0_bm; } while(0)
    
#define DBG_OUT_SetHigh() do { PORTD.OUTSET = PIN2_bm; } while(0)
#define DBG_OUT_SetLow() do { PORTD.OUTCLR = PIN2_bm; } while(0)
#define DBG_OUT_Toggle() do { PORTD.OUTTGL = PIN2_bm; } while(0)


    //Inits. General Purpose I/O
    void GPIO_init(void);


#ifdef	__cplusplus
}
#endif

#endif	/* GPIO_H */

