#ifndef MLX90392_H
#define	MLX90392_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
    
    typedef enum {
        STAT1 = 0x00, XRES_L, XRES_H, YRES_L, YRES_H, ZRES_L, ZRES_H, STAT2, TEMP_L, TEMP_H, CID, DID, 
                CTRL = 0x10, RST, 
                OSC_DIG_FILT = 0x14, T_EN_DIG_FILT_Z
    } MLX90392_Registers;
    
    typedef enum {
        IDLE = 0, SINGLE, CONT_10HZ, CONT_20HZ, CONT_50HZ, CONT_100HZ, SELF_TEST, 
                CONT_200HZ = 10, CONT_500HZ, CONT_800HZ, CONT_1400HZ,
                POWER_DOWN = 15
    } MLX90392_Mode;
    
    //Result Data type - encloses the first 8 fields on the device.
    typedef union {
        struct {
            
            //Status 1 Register
            union
            {
               struct {
                unsigned : 4;
                unsigned RT : 1;
                unsigned : 2;
                unsigned DRDY : 1;
                };
                uint8_t status1_byte;
            } status1;
            
            //Magnetic Measurements
            int16_t X_Meas;
            int16_t Y_Meas;
            int16_t Z_Meas;
            
            //Status 2 Register
            union{
                struct{
                    unsigned : 6;
                    unsigned DOR : 1;
                    unsigned HOVF : 1;
                };
                uint8_t status2_byte;
            } status2;
        };
        
        //Bytes to allocate - easier to load into memory this way
        uint8_t data[8];
    } MLX90393_Result;        
    
//Self-Test Ranges
#define MLX90392_X_LOW_PASS -150
#define MLX90392_Y_LOW_PASS -150
#define MLX90392_Z_LOW_PASS -1200
    
#define MLX90392_X_HIGH_PASS 150
#define MLX90392_Y_HIGH_PASS 150
#define MLX90392_Z_HIGH_PASS 400
    
    bool MLX90392_reset(void);   //Returns false if failed to init
    bool MLX90392_isDataReady(void);    //Returns true if data is ready
    bool MLX90392_selfTest(void);   //Returns true if self-test passed, false if failed
    
    bool MLX90392_setOperatingMode(MLX90392_Mode mode);
    
    bool MLX90392_getSingleMeasurement(MLX90393_Result* result);
    
    bool MLX90392_getResult(MLX90393_Result* result);
    bool MLX90392_getTemp(uint16_t* temp);
    
#ifdef	__cplusplus
}
#endif

#endif	/* MLX90392_H */

