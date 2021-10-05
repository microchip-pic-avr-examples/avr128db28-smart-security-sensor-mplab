/**
 * \file
 * twi0.c
 *
 * \ingroup
 * i2c0_host
 *
 * \brief
 *  This file contains the driver code for TWI0 module.
 *
 * \version
 * I2C Host Driver Version 2.0.0
 */

/*
© [2021] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include "../twi0.h"
#include <stdbool.h>
#include <stdlib.h>

static void I2C0_Close(void);
static void I2C0_ReadStart(void);
static void I2C0_WriteStart(void);
static void I2C0_EventHandler(void);
static void I2C0_ErrorEventHandler(void);
static void I2C0_DefaultCallback(void);

/* I2C Internal API's */
static uint8_t I2C0_GetRxData(void);
static void I2C0_SendTxData(uint8_t data);
static void I2C0_SendTxAddr(uint8_t data);
static inline void I2C0_ClearInterruptFlag(void);
static inline void I2C0_ResetBus(void);
static inline void I2C0_EnableRestart(void);
static inline void I2C0_DisableRestart(void);
static inline void I2C0_SendStop(void);
static bool I2C0_IsNack(void);
static bool I2C0_IsBusError(void);
static bool I2C0_IsData(void);
static bool I2C0_IsAddr(void);
static inline void I2C0_ClearErrorFlags(void);
static inline void I2C0_EnableInterrupts(void);
static inline void I2C0_DisableInterrupts(void);
static inline void I2C0_ClearInterrupts(void);
static inline void I2C0_HostSendAck(void);
static inline void I2C0_HostSendNack(void);
static bool I2C0_IsArbitrationlostOverride(void);

/* Host */
/* I2C0 interfaces */
static i2c_event_states_t I2C_EVENT_IDLE(void);
static i2c_event_states_t I2C_EVENT_SEND_RD_ADDR(void);
static i2c_event_states_t I2C_EVENT_SEND_WR_ADDR(void);
static i2c_event_states_t I2C_EVENT_TX(void);
static i2c_event_states_t I2C_EVENT_RX(void);
static i2c_event_states_t I2C_EVENT_NACK(void);
static i2c_event_states_t I2C_EVENT_ERROR(void);
static i2c_event_states_t I2C_EVENT_STOP(void);
static i2c_event_states_t I2C_EVENT_RESET(void);

/**
  Section: Driver Interface
*/
const i2c_host_interface_t i2c0_host_interface = {
  .Initialize = I2C0_Initialize,
  .Deinitialize = I2C0_Deinitialize,
  .Write = I2C0_Write,
  .Read = I2C0_Read,
  .WriteRead = I2C0_WriteRead,
  .TransferSetup = NULL,
  .ErrorGet = I2C0_ErrorGet,
  .IsBusy = I2C0_IsBusy,
  .CallbackRegister = I2C0_CallbackRegister,
  .Tasks = I2C0_Tasks
};

/**
 Section: Private Variable Definitions
 */

static void (*I2C0_Callback)(void) = NULL;
volatile i2c_event_status_t I2C0_Status = {0};

typedef i2c_event_states_t (*i2c0eventHandler)(void);
const i2c0eventHandler i2c0_eventTable[] = {
    I2C_EVENT_IDLE,
    I2C_EVENT_SEND_RD_ADDR,
    I2C_EVENT_SEND_WR_ADDR,
    I2C_EVENT_TX,
    I2C_EVENT_RX,
    I2C_EVENT_NACK,
    I2C_EVENT_ERROR,
    I2C_EVENT_STOP,
    I2C_EVENT_RESET
};

void I2C0_Initialize(void)
{
    //FMPEN disabled; INPUTLVL I2C; SDAHOLD OFF; SDASETUP 4CYC; 
    TWI0.CTRLA = 0x0;
    
    //Debug Run
    TWI0.DBGCTRL = 0x0;
    
    //Host Baud Rate Control
    TWI0.MBAUD = (uint8_t)I2C0_BAUD(100000, 0);
    
    //ENABLE enabled; QCEN disabled; RIEN false; SMEN disabled; TIMEOUT DISABLED; WIEN false; 
    TWI0.MCTRLA = 0x1;
    
    //ARBLOST disabled; BUSERR disabled; BUSSTATE UNKNOWN; CLKHOLD disabled; RIF disabled; WIF disabled; 
    TWI0.MSTATUS = 0x0;
    
    //Host Address
    TWI0.MADDR = 0x0;
    
    //ACKACT ACK; FLUSH disabled; MCMD NOACT; 
    TWI0.MCTRLB = 0x0;
    
    //Host Data
    TWI0.MDATA = 0x0;
    

    TWI0.MCTRLB |= TWI_FLUSH_bm; 
    TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;

    I2C0_CallbackRegister(I2C0_DefaultCallback);
}

void I2C0_Deinitialize(void)
{
    //FMPEN disabled; INPUTLVL I2C; SDAHOLD OFF; SDASETUP 4CYC; 
    TWI0.CTRLA = 0x00;
    
    //Debug Run
    TWI0.DBGCTRL = 0x00;
    
    //Host Baud Rate Control
    TWI0.MBAUD = (uint8_t)I2C0_BAUD(100000, 0);
    
    //ENABLE enabled; QCEN disabled; RIEN false; SMEN disabled; TIMEOUT DISABLED; WIEN false; 
    TWI0.MCTRLA = 0x00;
    
    //ARBLOST disabled; BUSERR disabled; BUSSTATE UNKNOWN; CLKHOLD disabled; RIF disabled; WIF disabled; 
    TWI0.MSTATUS = 0x00;
    
    //Host Address
    TWI0.MADDR = 0x00;
    
    //ACKACT ACK; FLUSH disabled; MCMD NOACT; 
    TWI0.MCTRLB = 0x00;
    
    //Host Data
    TWI0.MDATA = 0x00;

    TWI0.MCTRLB |= TWI_FLUSH_bm; 
    TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;

    I2C0_CallbackRegister(I2C0_DefaultCallback);
    
}

bool I2C0_Write(uint16_t address, uint8_t *data, size_t dataLength)
{
    bool retStatus = false;
    if (!I2C0_IsBusy())
    {
        I2C0_Status.busy = true;
        I2C0_Status.address = address;
        I2C0_Status.switchToRead = false;
        I2C0_Status.writePtr = data;
        I2C0_Status.writeLength = dataLength;
        I2C0_Status.readPtr = NULL;
        I2C0_Status.readLength = 0;
        I2C0_Status.errorState = I2C_ERROR_NONE;
        I2C0_WriteStart();
        retStatus = true;
    }
    return retStatus;
}

bool I2C0_Read(uint16_t address, uint8_t *data, size_t dataLength)
{
    bool retStatus = false;
    if (!I2C0_IsBusy())
    {
        I2C0_Status.busy = true;
        I2C0_Status.address = address;
        I2C0_Status.switchToRead = false;
        I2C0_Status.readPtr = data;
        I2C0_Status.readLength = dataLength;
        I2C0_Status.writePtr = NULL;
        I2C0_Status.writeLength = 0;
        I2C0_Status.errorState = I2C_ERROR_NONE;
        I2C0_ReadStart();
        retStatus = true;
    }
    return retStatus;
}

bool I2C0_WriteRead(uint16_t address, uint8_t *writeData, size_t writeLength, uint8_t *readData, size_t readLength)
{
    bool retStatus = false;
    if (!I2C0_IsBusy())
    {
        I2C0_Status.busy = true;
        I2C0_Status.address = address;
        I2C0_Status.switchToRead = true;
        I2C0_Status.writePtr = writeData;
        I2C0_Status.writeLength = writeLength;
        I2C0_Status.readPtr = readData;
        I2C0_Status.readLength = readLength;
        I2C0_Status.errorState = I2C_ERROR_NONE;
        I2C0_WriteStart();
        retStatus = true;
    }
    return retStatus;
}

i2c_host_error_t I2C0_ErrorGet(void)
{
    i2c_host_error_t retErrorState = I2C0_Status.errorState;
    I2C0_Status.errorState = I2C_ERROR_NONE;
    return retErrorState;
}

bool I2C0_IsBusy(void)
{
    return I2C0_Status.busy || !(TWI0.MSTATUS & TWI_BUSSTATE_IDLE_gc);
}

void I2C0_CallbackRegister(void (*callbackHandler)(void))
{
    if (callbackHandler != NULL)
    {
        I2C0_Callback = callbackHandler;
    }
}

void I2C0_Tasks(void)
{
    bool retStatus = I2C0_IsBusy();
    if (retStatus)
    {
        if(((TWI0.MSTATUS & TWI_RXACK_bm) && (TWI0.MSTATUS & TWI_WIF_bm) && (!(TWI0.MSTATUS & TWI_BUSERR_bm)) && (!(TWI0.MSTATUS & TWI_ARBLOST_bm))) || (TWI0.MSTATUS & TWI_BUSERR_bm) || (TWI0.MSTATUS & TWI_ARBLOST_bm))
        {
            I2C0_ErrorEventHandler();
        }
        if ((TWI0.MSTATUS & TWI_RIF_bm) || (TWI0.MSTATUS & TWI_WIF_bm))
        {
            if ((TWI0.MSTATUS & TWI_RXACK_bm) || (TWI0.MSTATUS & TWI_BUSERR_bm) || (TWI0.MSTATUS & TWI_ARBLOST_bm))
            {
                I2C0_ErrorEventHandler();
            }
            else
            {
                I2C0_EventHandler();
            }
        }
    }
}

/**
 Section: Private Interfaces
 */
static void I2C0_ReadStart(void)
{
    I2C0_Status.state = I2C_EVENT_SEND_RD_ADDR();
}

static void I2C0_WriteStart(void)
{
    I2C0_Status.state = I2C_EVENT_SEND_WR_ADDR();
}

static void I2C0_Close(void)
{
    I2C0_Status.busy = false;
    I2C0_Status.address = 0xFF;
    I2C0_Status.writePtr = NULL;
    I2C0_Status.readPtr = NULL;
    I2C0_Status.state = I2C_STATE_IDLE;
    I2C0_ClearInterrupts();
    I2C0_DisableInterrupts();
    TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
}

static void I2C0_EventHandler(void)
{
    I2C0_Status.state = i2c0_eventTable[I2C0_Status.state]();
}

static void I2C0_ErrorEventHandler(void)
{
    if (I2C0_IsBusError())
    {
        I2C0_Status.state = I2C_STATE_ERROR;
        I2C0_Status.errorState = I2C_ERROR_BUS_COLLISION;
        TWI0.MSTATUS |= TWI_BUSERR_bm;
    }
    else if (I2C0_IsAddr() && I2C0_IsNack())
    {
        I2C0_Status.state = I2C_STATE_NACK;
        I2C0_Status.errorState = I2C_ERROR_ADDR_NACK;
        TWI0.MSTATUS |= TWI_RXACK_bm;
    }
    else if (I2C0_IsData() && I2C0_IsNack())
    {
        I2C0_Status.state = I2C_STATE_NACK;
        I2C0_Status.errorState = I2C_ERROR_DATA_NACK;
        TWI0.MSTATUS |= TWI_RXACK_bm;
    }
    else if(I2C0_IsArbitrationlostOverride())
    {
        I2C0_Status.state = I2C_STATE_ERROR;
        I2C0_Status.errorState = I2C_ERROR_BUS_COLLISION;
        TWI0.MSTATUS |= TWI_ARBLOST_bm;
    }
    I2C0_Status.state = i2c0_eventTable[I2C0_Status.state]();
    if(I2C0_Status.errorState != I2C_ERROR_NONE)
    {
        I2C0_Callback();
    }
}

static void I2C0_DefaultCallback(void)
{
    // Default Callback for Error Indication
}

/* I2C0 Event interfaces */
static i2c_event_states_t I2C_EVENT_IDLE(void)
{
    I2C0_Status.busy = false;
    TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
    return I2C_STATE_RESET;
}

static i2c_event_states_t I2C_EVENT_SEND_RD_ADDR(void)
{
    I2C0_SendTxAddr((uint8_t) (I2C0_Status.address << 1 | 1));
    return I2C_STATE_RX;
}

static i2c_event_states_t I2C_EVENT_SEND_WR_ADDR(void)
{
    I2C0_SendTxAddr((uint8_t) (I2C0_Status.address << 1));
    return I2C_STATE_TX;
}

static i2c_event_states_t I2C_EVENT_TX(void)
{
    i2c_event_states_t retEventState = I2C_STATE_TX;

        if (I2C0_Status.writeLength)
        {
            I2C0_Status.writeLength--;
            I2C0_SendTxData(*I2C0_Status.writePtr++);
            retEventState = I2C_STATE_TX;
        }
        else
        {
            if (I2C0_Status.switchToRead)
            {
                I2C0_Status.switchToRead = false;
                retEventState = I2C_STATE_SEND_RD_ADDR;
            }
            else
            {
                retEventState = I2C_EVENT_STOP();
            }
        }

    return retEventState;
}

static i2c_event_states_t I2C_EVENT_RX(void)
{
    i2c_event_states_t retEventState = I2C_STATE_RX;

    if (I2C0_Status.readLength == 1)
    {
        // Next byte will be last to be received, setup NACK
        I2C0_HostSendNack();
    }
    else
    {
        // More bytes to receive, setup ACK
        I2C0_HostSendAck();
    }

    if (--I2C0_Status.readLength)
    {
        *I2C0_Status.readPtr++ = I2C0_GetRxData();
        //Execute Acknowledge Action followed by a byte read operation
        TWI0.MCTRLB |= TWI_MCMD_RECVTRANS_gc;
        retEventState = I2C_STATE_RX;
    }
    else
    {
        *I2C0_Status.readPtr++ = I2C0_GetRxData();
        I2C0_HostSendNack();
        retEventState = I2C_EVENT_STOP();
    }

    return retEventState;
}

static i2c_event_states_t I2C_EVENT_NACK(void)
{
    i2c_event_states_t retEventState = I2C_STATE_NACK;
    retEventState = I2C_EVENT_STOP();
    return retEventState;
}

static i2c_event_states_t I2C_EVENT_ERROR(void)
{
    // Clear bus collision status flag
    i2c_event_states_t retEventState = I2C_STATE_ERROR;
    I2C0_ClearInterrupts();
    retEventState = I2C_EVENT_RESET();
    return retEventState;
}

static i2c_event_states_t I2C_EVENT_STOP(void)
{
    I2C0_SendStop();
    I2C0_Close();
    return I2C_STATE_IDLE;
}

static i2c_event_states_t I2C_EVENT_RESET(void)
{
    I2C0_ResetBus();
    I2C0_Status.busy = false;
    return I2C_STATE_IDLE;
}

/**
 Section: Register Level Interfaces
 */
static uint8_t I2C0_GetRxData(void)
{
    return TWI0.MDATA;
}

static void I2C0_SendTxData(uint8_t data)
{
    TWI0.MDATA = data;
}

static void I2C0_SendTxAddr(uint8_t data)
{
    TWI0.MADDR = data;
}

static inline void I2C0_ResetBus(void)
{
    //Set Clear Buffer Flag
    TWI0.MCTRLA &= ~(1 << TWI_ENABLE_bp);
    TWI0.MCTRLA |= 1 << TWI_ENABLE_bp;
    TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
}

static void I2C0_ClearInterruptFlag(void)
{
    if(TWI0.MSTATUS & TWI_RIF_bm)
    {
        TWI0.MSTATUS|= TWI_RIF_bm;
    }
    else if(TWI0.MSTATUS & TWI_WIF_bm)
    {
        TWI0.MSTATUS|= TWI_WIF_bm;
    }
}

static inline void I2C0_EnableRestart(void)
{
    //Enable I2C0 Restart
    TWI0.MCTRLB |= TWI_MCMD_REPSTART_gc;
}

static inline void I2C0_DisableRestart(void)
{
    //Disable I2C0 Restart
    TWI0.MCTRLB &= ~TWI_MCMD_REPSTART_gc;
}

static inline void I2C0_SendStop(void)
{
    //Clear Start Bit
    TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

static bool I2C0_IsNack(void)
{
    return TWI0.MSTATUS & TWI_RXACK_bm;
}

static bool I2C0_IsBusError(void)
{
    return TWI0.MSTATUS & TWI_BUSERR_bm;
}

static bool I2C0_IsData(void)
{
    return TWI0.MDATA;
}

static bool I2C0_IsAddr(void)
{
    return TWI0.MADDR;
}

static bool I2C0_IsArbitrationlostOverride(void)
{
    return TWI0.MSTATUS & TWI_ARBLOST_bm;
}

static inline void I2C0_ClearErrorFlags(void)
{
   TWI0.MSTATUS |= TWI_RXACK_bm;
   TWI0.MSTATUS |= TWI_BUSERR_bm;
   TWI0.MSTATUS |= TWI_ARBLOST_bm;
}

static inline void I2C0_HostSendAck(void)
{
    TWI0.MCTRLB &= ~(1 << TWI_ACKACT_bp);
}

static inline void I2C0_HostSendNack(void)
{
    TWI0.MCTRLB |= TWI_ACKACT_NACK_gc;
}

static inline void I2C0_EnableInterrupts(void)
{
    TWI0.MCTRLA |= (TWI_RIEN_bm | TWI_WIEN_bm);
}

static inline void I2C0_DisableInterrupts(void)
{
    TWI0.MCTRLA &= ~(TWI_RIEN_bm | TWI_WIEN_bm);
}

static inline void I2C0_ClearInterrupts(void)
{
   TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);
}
