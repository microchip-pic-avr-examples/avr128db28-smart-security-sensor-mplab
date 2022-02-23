#include "MVIO.h"

#include <stdbool.h>
#include <avr/interrupt.h>

static void (*handler)(void);

void MVIO_init(void)
{
    //Clear callback function
    handler = 0;
    
    //Enable MVIO Interrupts
    //MVIO.INTCTRL = MVIO_VDDIO2IE_bm;
}

//Sets the callback function for MVIO Change Events
void MVIO_setCallback(void(*func)(void))
{
    handler = func;
}

//Returns the current state of the MVIO
bool MVIO_isOK(void)
{
    return (MVIO.STATUS & MVIO_VDDIO2S_bm);
}

ISR(MVIO_MVIO_vect)
{
    //If callback is set...
    if (handler)
    {
        handler();
    }
    
    //Clear MVIO Flag
    MVIO.INTFLAGS |= MVIO_VDDIO2IF_bm;
}
