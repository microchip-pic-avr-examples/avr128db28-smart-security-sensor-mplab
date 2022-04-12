#ifndef LEDCONTROL_H
#define	LEDCONTROL_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    //Turns on/off the Red channel
    void LED_turnOnRed(void);
    void LED_turnOffRed(void);
    
    //Turns on/off the Green channel
    void LED_turnOnGreen(void);
    void LED_turnOffGreen(void);

    //Turns on/off the Blue channel
    void LED_turnOnBlue(void);
    void LED_turnOffBlue(void);
    
    //Switches from PWM to IO Control
    void LED_switchToSleep(void);
    
    //Switches from IO to PWM Control
    void LED_switchToActive(void);

#ifdef	__cplusplus
}
#endif

#endif	/* LEDCONTROL_H */

