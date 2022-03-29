#ifndef DEFAULTS_H
#define	DEFAULTS_H

#ifdef	__cplusplus
extern "C" {
#endif

//Default Result Rate (15s))
#define DEFAULT_RTC_PERIOD 0x0F00
        
//Default Time in the RTC Period to Start
#define DEFAULT_RTC_COMPARE 0x0DC0
    
//Default Temp Unit
#define DEFAULT_TEMP_UNIT 'C'
    
//Default High Temp Warning (90F/32.2C). Value must be in C
#define DEFAULT_TEMP_WARNING_H 32.2

//Default High Temp Warning (50F/10C). Value must be in C
#define DEFAULT_TEMP_WARNING_L 10.0

#ifdef	__cplusplus
}
#endif

#endif	/* DEFAULTS_H */
