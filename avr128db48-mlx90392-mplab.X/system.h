#ifndef SYSTEM_H
#define	SYSTEM_H

#ifdef	__cplusplus
extern "C" {
#endif

    //Inits CLKCTRL, EVSYS, WDT, and SLPCTRL
    void System_initDevice(void);

    //Inits peripherals
    void System_initPeripherals(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_H */

