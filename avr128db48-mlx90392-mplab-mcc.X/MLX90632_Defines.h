#ifndef MLX90632_DEFINES_H
#define	MLX90632_DEFINES_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MLX90632_I2C_ADDR_BASE 0x3A
    
//Powers of 2
#define EXP_2_N8        3.90625e-3F                             //2^-8
#define EXP_2_N10       9.765625e-4F                            //2^-10     
#define EXP_2_N14       6.103515625e-5F                         //2^-14
#define EXP_2_N16       1.52587890625e-5F                       //2^-16         
#define EXP_2_N20       9.5367431640625e-7F                     //2^-20 
#define EXP_2_N36       1.4551915228366851806640625e-11F        //2^-36
#define EXP_2_N44       5.684341886080801486968994140625e-14F   //2^-44  
#define EXP_2_N46       1.421085471520200372e-14F               //2^-46                       

#define EXP_2_P19       524288                                  //2^19
    
//Multipliers for each constant
#define P_R_MULT    EXP_2_N8
#define P_G_MULT    EXP_2_N20
#define P_T_MULT    EXP_2_N44
#define P_O_MULT    EXP_2_N8                    
#define EA_MULT     EXP_2_N16
#define EB_MULT     EXP_2_N8
#define FA_MULT     EXP_2_N46
#define FB_MULT     EXP_2_N36
#define GA_MULT     EXP_2_N36
#define GB_MULT     EXP_2_N10
#define KA_MULT     EXP_2_N10
#define HA_MULT     EXP_2_N14
#define HB_MULT     EXP_2_N10
    
    //Special Data Type for Storing / Loading Floats
    typedef union {
        float value;
        uint32_t hexCode;
    } float_hex;
        
    //Register Addresses
    typedef enum {
        MLX90632_ID0 = 0x2405, MLX90632_ID1, MLX90632_ID2, MLX90632_ID_CRC16, MLX90632_EE_PRODUCT_CODE,
        MLX90632_EE_VERSION = 0x240B, EE_P_R_LOW, EE_P_R_HIGH, EE_P_G_LOW, EE_P_G_HIGH,
        EE_P_T_LOW, EE_P_T_HIGH, EE_P_O_LOW, EE_P_O_HIGH, EE_AA_LOW, EE_AA_HIGH,
        EE_AB_LOW, EE_AB_HIGH, EE_BA_LOW, EE_BA_HIGH, EE_BB_LOW, EE_BB_HIGH,
        EE_CA_LOW, EE_CA_HIGH, EE_CB_LOW, EE_CB_HIGH, EE_DA_LOW, EE_DA_HIGH,
        EE_DB_LOW, EE_DB_HIGH, EE_EA_LOW, EE_EA_HIGH, EE_EB_LOW, EE_EB_HIGH,
        EE_FA_LOW, EE_FA_HIGH, EE_FB_LOW, EE_FB_HIGH, EE_GA_LOW, EE_GA_HIGH,
        EE_GB, EE_KA, EE_KB,
        EE_HA_LOW = 0x2481, EE_HA_HIGH,
        MLX90632_CUSTOMER_DATA_START = 0x24C0,
        MLX90632_EE_CONTROL = 0x24D4, MLX90632_EE_I2C_ADDRESS,
        MLX90632_EE_MEAS_1 = 0x24E1, MLX90632_EE_MEAS_2,
        MLX90632_REG_I2C_ADDRESS = 0x3000, MLX90632_REG_CONTROL,
        MLX90632_REG_STATUS = 0x3FFF,
        MLX90632_RAM_START = 0x4000, MLX90632_RAM4_START = 0x4003
    } MLX90632_Register;

    typedef union {
        struct
        {
            unsigned : 6;
            unsigned FOV : 2;
            unsigned PACKAGE : 3;
            unsigned ACCURACY : 5;
        };
        
        uint16_t regValue;
        
    } MLX90632_ProductCode;
        
#define MLX90632_PRODUCT_FOV_gp 8
#define MLX90632_PRODUCT_FOV gm (0b11 << MLX90632_PRODUCT_FOV_gp)
    
#define MLX90632_PRODUCT_FOV_70D 1
    
#define MLX90632_PRODUCT_PACKAGE_gp 5
#define MLX90632_PRODUCT_PACKAGE_gm (0b111 << MLX90632_PRODUCT_PACKAGE_gp)

#define MLX90632_PRODUCT_PACKAGE_SFN 1
    
#define MLX90632_PRODUCT_ACCURACY_gp 0
#define MLX90632_PRODUCT_ACCURACY_gm (0b11111 << MLX90632_PRODUCT_ACCURACY_gp)
    
#define MLX90632_PRODUCT_ACCURACY_MEDICAL 1
#define MLX90632_PRODUCT_ACCURACY_STANDARD 2
    
    typedef union {
        struct
        {
            unsigned : 4;
            unsigned SOB : 1;
            unsigned : 2;
            unsigned MEAS_SELECT : 5;
            unsigned SOC : 1;
            unsigned MODE : 2;
            unsigned : 1;
        };
        
        uint16_t regValue;
    } MLX90632_Control;
    
#define MLX90632_CONTROL_SOB_bp 11
#define MLX90632_CONTROL_SOB_bm (0b1 << MLX90632_CONTROL_SOB_bp)
    
#define MLX90632_CONTROL_MEAS_SELECT_gp 4
#define MLX90632_CONTROL_MEAS_SELECT_gm (0b11111 << MLX90632_CONTROL_MEAS_SELECT_gp)
    
#define MLX90632_CONTROL_MEAS_SELECT_MEDICAL 0x00
#define MLX90632_CONTROL_MEAS_SELECT_EXTENDED 0x11
    
#define MLX90632_CONTROL_SOC_bp 3
#define MLX90632_CONTROL_SOC_bm (0b1 << MLX90632_CONTROL_SOC_bp)
    
#define MLX90632_CONTROL_MODE_gp 1
#define MLX90632_CONTROL_MODE_gm (0b11 << MLX90632_CONTROL_MODE_gp)
    
#define MLX90632_CONTROL_MODE_SLEEPING_STEP 0b01
#define MLX90632_CONTROL_MODE_STEP 0b10
#define MLX90632_CONTROL_MODE_CONTINUOUS 0b11

    
    typedef union {
        struct{
            unsigned : 5;
            unsigned DEVICE_BUSY : 1;
            unsigned EEPROM_BUSY : 1;
            unsigned BROWN_OUT : 1;
            unsigned : 1;
            unsigned CYCLE_POSITION : 5;
            unsigned : 1;
            unsigned NEW_DATA : 1;
        };
        
        uint16_t regValue;
        
    } MLX90632_Status;
    
#define MLX90632_STATUS_DEVICE_BUSY_bp 10
#define MLX90632_STATUS_DEVICE_BUSY_bm (0b1 << MLX90632_STATUS_DEVICE_BUSY_bp)

#define MLX90632_STATUS_EEPROM_BUSY_bp 9
#define MLX90632_STATUS_EEPROM_BUSY_bm (0b1 << MLX90632_STATUS_EEPROM_BUSY_bp)
    
#define MLX90632_STATUS_BROWN_OUT_bp 8
#define MLX90632_STATUS_BROWN_OUT_bm (0b1 << MLX90632_STATUS_BROWN_OUT_bp)
    
#define MLX90632_STATUS_CYCLE_POSITION_gp 2
#define MLX90632_STATUS_CYCLE_POSITION_gm (0b11111 << MLX90632_STATUS_CYCLE_POSITION_gp)
    
#define MLX90632_STATUS_NEW_DATA_bp 0
#define MLX90632_STATUS_NEW_DATA_bm (0b1 << MLX90632_STATUS_NEW_DATA_bp)
    
    typedef union {
        struct {
            unsigned : 5;
            unsigned REFRESH_RATE : 3;
            unsigned : 8;
        };
        
        uint16_t regValue;
    } MLX90632_RefreshRate;

#define MLX90632_REFRESH_RATE_gp 8
#define MLX90632_REFRESH_RATE_gm (0b111 << MLX90632_REFRESH_RATE_gp)

    
#ifdef	__cplusplus
}
#endif

#endif	/* MLX90632_DEFINES_H */

