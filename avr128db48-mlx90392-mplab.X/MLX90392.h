#ifndef MLX90392_H
#define	MLX90392_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
    
    typedef enum {
        MLX90392_STAT1 = 0x00, MLX90392_XRES_L, MLX90392_XRES_H, MLX90392_YRES_L, 
                MLX90392_YRES_H, MLX90392_ZRES_L, MLX90392_ZRES_H, MLX90392_STAT2,
                MLX90392_TEMP_L, MLX90392_TEMP_H, MLX90392_COMPANY_ID, 
                MLX90392_DEVICE_ID, 
                MLX90392_CTRL = 0x10, MLX90392_RST, 
                MLX90392_OSC_DIG_FILT = 0x14, MLX90392_CUST_CTRL
    } MLX90392_Register;
    
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
    } MLX90392_RawResult16;        
    
//STAT1
#define MLX90392_STAT1_RT_bp 3
#define MLX90392_STAT1_RT_bm (1 << MLX90392_STAT1_RT_bp)
#define MLX90392_STAT1_DRDY_bp 0
#define MLX90392_STAT1_DRDY_bm (1 << MLX90392_STAT1_DRDY_bp)

//STAT2
#define MLX90392_STAT2_DOR_bp 1
#define MLX90392_STAT2_DOR_bm (1 << MLX90392_STAT2_DOR_bp)
#define MLX90392_STAT2_HOVF_bp 0
#define MLX90392_STAT2_HOVF_bm (1 << MLX90392_STAT2_HOVF_bp)

//CTRL Register
#define MLX90392_CTRL_MODE_gp 0
#define MLX90392_CTRL_MODE_gm (0b1111 << MLX90392_CTRL_MODE_gp)
    
//OSR_DIG_FILT Register
#define MLX90392_OSR_DIG_FILT_OSR_HALL_bp 7
#define MLX90392_OSR_DIG_FILT_OSR_HALL_bm (1 << MLX90392_OSR_DIG_FILT_OSR_HALL_bp)
#define MLX90392_OSR_DIG_FILT_OSR_TEMP_bp 6
#define MLX90392_OSR_DIG_FILT_OSR_TEMP_bm (1 << MLX90392_OSR_DIG_FILT_OSR_TEMP_bp)
#define MLX90392_OSR_DIG_FILT_OSR_HALL_XY_gp 3
#define MLX90392_OSR_DIG_FILT_OSR_HALL_XY_gm (0b111 << MLX90392_OSR_DIG_FILT_OSR_HALL_XY_gp)
#define MLX90392_OSR_DIG_FILT_OSR_TEMP_MP_gp 0
#define MLX90392_OSR_DIG_FILT_OSR_TEMP_MP_gm (0b111 << MLX90392_OSR_DIG_FILT_OSR_TEMP_MP_gp)
    
//CUST_CTRL Register
#define MLX90392_CUST_CTRL_DNC 0x90         //Do not change values. Always OR this with CUST_CTRL
#define MLX90392_CUST_CTRL_T_COMP_EN_bp 5
#define MLX90392_CUST_CTRL_T_COMP_EN_bm (1 << MLX90392_CUST_CTRL_T_COMP_EN_bp)
#define MLX90392_CUST_CTRL_DIG_FILT_HALL_Z_gp 0
#define MLX90392_CUST_CTRL_DIG_FILT_HALL_Z_gm (0b111 << MLX90392_CUST_CTRL_DIG_FILT_HALL_Z_gp)
    
//Self-Test Ranges
#define MLX90392_X_LOW_PASS -150
#define MLX90392_Y_LOW_PASS -150
#define MLX90392_Z_LOW_PASS -1200
    
#define MLX90392_X_HIGH_PASS 150
#define MLX90392_Y_HIGH_PASS 150
#define MLX90392_Z_HIGH_PASS -400
    
    //Init the Sensor
    bool MLX90392_init(void);
    
    //Returns the register specified from the device
    bool MLX90392_getRegister(MLX90392_Register reg, uint8_t* data);
    
    //Sets the register specified on the device
    bool MLX90392_setRegister(MLX90392_Register reg, uint8_t data);
    
    //Resets the MLX90392 Sensor.
    //Returns true if successful or false if failed
    bool MLX90392_reset(void);
    
    //Returns true if data is ready to be read
    bool MLX90392_isDataReady(void); 
    
    //Returns true if passes self-test. ONLY for -10 sensor variant
    bool MLX90392_selfTest(void); 
    
    //Sets the Operating Mode of the Sensor
    bool MLX90392_setOperatingMode(MLX90392_Mode mode);
    
    //Sets the operating mode of the sensor, waits, then returns a result
    bool MLX90392_getSingleMeasurementBlocking(MLX90392_RawResult16* result);
    
    //Reads the result registers on the sensor
    bool MLX90392_getResult(MLX90392_RawResult16* result);
    
    //Returns the temperature of the sensor
    bool MLX90392_getTemp(uint16_t* temp);
    
#ifdef	__cplusplus
}
#endif

#endif	/* MLX90392_H */

