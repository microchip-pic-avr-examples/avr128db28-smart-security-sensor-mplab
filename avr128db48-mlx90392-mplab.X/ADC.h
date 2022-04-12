#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif
   
#include <stdint.h>
        
    //Init the ADC
    void ADC_init(void);
    
    //Trigger a conversion
    uint16_t ADC_getValue(uint8_t channel);
    
    //Trigger a conversion and return it in float
    float ADC_getResultAsFloat(uint8_t channel);

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

