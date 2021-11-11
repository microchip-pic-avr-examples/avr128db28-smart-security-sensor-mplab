/**
 * \file
 * twi1.c
 *
 * \ingroup
 * i2c1_host
 *
 * \brief
 *  This file contains the driver code for TWI1 module.
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

#include "../twi1.h"
#include <stdbool.h>
#include <stdlib.h>

static void I2C1_Close(void);
static void I2C1_ReadStart(void);
static void I2C1_WriteStart(void);
static void I2C1_EventHandler(void);
static void I2C1_ErrorEventHandler(void);
static void I2C1_DefaultCallback(void);

/* I2C Internal API's */
static uint8_t I2C1_GetRxData(void);
static void I2C1_SendTxData(uint8_t data);
static void I2C1_SendTxAddr(uint8_t data);
static inline void I2C1_ClearInterruptFlag(void);
static inline void I2C1_ResetBus(void);
static inline void I2C1_EnableRestart(void);
static inline void I2C1_DisableRestart(void);
static inline void I2C1_SendStop(void);
static bool I2C1_IsNack(void);
static bool I2C1_IsBusError(void);
static bool I2C1_IsData(void);
static bool I2C1_IsAddr(void);
static inline void I2C1_ClearErrorFlags(void);
static inline void I2C1_EnableInterrupts(void);
static inline void I2C1_DisableInterrupts(void);
static inline void I2C1_ClearInterrupts(void);
static inline void I2C1_HostSendAck(void);
static inline void I2C1_HostSendNack(void);
static bool I2C1_IsArbitrationlostOverride(void);

/* Host */
/* I2C1 interfaces */
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
const i2c_host_interface_t i2c1_host_interface = {
  .Initialize = I2C1_Initialize,
  .Deinitialize = I2C1_Deinitialize,
  .Write = I2C1_Write,
  .Read = I2C1_Read,
  .WriteRead = I2C1_WriteRead,
  .TransferSetup = NULL,
  .ErrorGet = I2C1_ErrorGet,
  .IsBusy = I2C1_IsBusy,
  .CallbackRegister = I2C1_CallbackRegister,
  .Tasks = I2C1_Tasks
};

/**
 Section: Private Variable Definitions
 */

static void (*I2C1_Callback)(void) = NULL;
volatile i2c_event_status_t I2C1_Status = {0};

typedef i2c_event_states_t (*i2c1eventHandler)(void);
const i2c1eventHandler i2c1_eventTable[] = {
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

void I2C1_Initialize(void)
{
    //FMPEN disabled; INPUTLVL I2C; SDAHOLD OFF; SDASETUP 4CYC; 
    TWI1.CTRLA = 0x0;
    
    //Debug Run
    TWI1.DBGCTRL = 0x0;
    
    //Host Baud Rate Control
    TWI1.MBAUD = (uint8_t)I2C1_BAUD(100000, 0);
    
    //ENABLE enabled; QCEN disabled; RIEN false; SMEN disabled; TIMEOUT DISABLED; WIEN false; 
    TWI1.MCTRLA = 0x1;
    
    //ARBLOST disabled; BUSERR disabled; BUSSTATE UNKNOWN; CLKHOLD disabled; RIF disabled; WIF disabled; 
    TWI1.MSTATUS = 0x0;
    
    //Host Address
    TWI1.MADDR = 0x0;
    
    //ACKACT ACK; FLUSH disabled; MCMD NOACT; 
    TWI1.MCTRLB = 0x0;
    
    //Host Data
    TWI1.MDATA = 0x0;
    

    TWI1.MCTRLB |= TWI_FLUSH_bm; 
    TWI1.MSTATUS |= TWI_BUSSTATE_IDLE_gc;

    I2C1_CallbackRegister(I2C1_DefaultCallback);
}

void I2C1_Deinitialize(void)
{
    //FMPEN disabled; INPUTLVL I2C; SDAHOLD OFF; SDASETUP 4CYC; 
    TWI1.CTRLA = 0x00;
    
    //Debug Run
    TWI1.DBGCTRL = 0x00;
    
    //Host Baud Rate Control
    TWI1.MBAUD = (uint8_t)I2C1_BAUD(100000, 0);
    
    //ENABLE enabled; QCEN disabled; RIEN false; SMEN disabled; TIMEOUT DISABLED; WIEN false; 
    TWI1.MCTRLA = 0x00;
    
    //ARBLOST disabled; BUSERR disabled; BUSSTATE UNKNOWN; CLKHOLD disabled; RIF disabled; WIF disabled; 
    TWI1.MSTATUS = 0x00;
    
    //Host Address
    TWI1.MADDR = 0x00;
    
    //ACKACT ACK; FLUSH disabled; MCMD NOACT; 
    TWI1.MCTRLB = 0x00;
    
    //Host Data
    TWI1.MDATA = 0x00;

    TWI1.MCTRLB |= TWI_FLUSH_bm; 
    TWI1.MSTATUS |= TWI_BUSSTATE_IDLE_gc;

    I2C1_CallbackRegister(I2C1_DefaultCallback);
    
}

bool I2C1_Write(uint16_t address, uint8_t *data, size_t dataLength)
{
    bool retStatus = false;
    if (!I2C1_IsBusy())
    {
        I2C1_Status.busy = true;
        I2C1_Status.address = address;
        I2C1_Status.switchToRead = false;
        I2C1_Status.writePtr = data;
        I2C1_Status.writeLength = dataLength;
        I2C1_Status.readPtr = NULL;
        I2C1_Status.readLength = 0;
        I2C1_Status.errorState = I2C_ERROR_NONE;
        I2C1_WriteStart();
        retStatus = true;
    }
    return retStatus;
}

bool I2C1_Read(uint16_t address, uint8_t *data, size_t dataLength)
{
    bool retStatus = false;
    if (!I2C1_IsBusy())
    {
        I2C1_Status.busy = true;
        I2C1_Status.address = address;
        I2C1_Status.switchToRead = false;
        I2C1_Status.readPtr = data;
        I2C1_Status.readLength = dataLength;
        I2C1_Status.writePtr = NULL;
        I2C1_Status.writeLength = 0;
        I2C1_Status.errorState = I2C_ERROR_NONE;
        I2C1_ReadStart();
        retStatus = true;
    }
    return retStatus;
}

bool I2C1_WriteRead(uint16_t address, uint8_t *writeData, size_t writeLength, uint8_t *readData, size_t readLength)
{
    bool retStatus = false;
    if (!I2C1_IsBusy())
    {
        I2C1_Status.busy = true;
        I2C1_Status.address = address;
        I2C1_Status.switchToRead = true;
        I2C1_Status.writePtr = writeData;
        I2C1_Status.writeLength = writeLength;
        I2C1_Status.readPtr = readData;
        I2C1_Status.readLength = readLength;
        I2C1_Status.errorState = I2C_ERROR_NONE;
        I2C1_WriteStart();
        retStatus = true;
    }
    return retStatus;
}

i2c_host_error_t I2C1_ErrorGet(void)
{
    i2c_host_error_t retErrorState = I2C1_Status.errorState;
    I2C1_Status.errorState = I2C_ERROR_NONE;
    return retErrorState;
}

bool I2C1_IsBusy(void)
{
    return I2C1_Status.busy || !(TWI1.MSTATUS & TWI_BUSSTATE_IDLE_gc);
}

void I2C1_CallbackRegister(void (*callbackHandler)(void))
{
    if (callbackHandler != NULL)
    {
        I2C1_Callback = callbackHandler;
    }
}

void I2C1_Tasks(void)
{
    bool retStatus = I2C1_IsBusy();
    if (retStatus)
    {
        if(((TWI1.MSTATUS & TWI_RXACK_bm) && (TWI1.MSTATUS & TWI_WIF_bm) && (!(TWI1.MSTATUS & TWI_BUSERR_bm)) && (!(TWI1.MSTATUS & TWI_ARBLOST_bm))) || (TWI1.MSTATUS & TWI_BUSERR_bm) || (TWI1.MSTATUS & TWI_ARBLOST_bm))
        {
            I2C1_ErrorEventHandler();
        }
        if ((TWI1.MSTATUS & TWI_RIF_bm) || (TWI1.MSTATUS & TWI_WIF_bm))
        {
            if ((TWI1.MSTATUS & TWI_RXACK_bm) || (TWI1.MSTATUS & TWI_BUSERR_bm) || (TWI1.MSTATUS & TWI_ARBLOST_bm))
            {
                I2C1_ErrorEventHandler();
            }
            else
            {
                I2C1_EventHandler();
            }
        }
    }
}

/**
 Section: Private Interfaces
 */
static void I2C1_ReadStart(void)
{
    I2C1_Status.state = I2C_EVENT_SEND_RD_ADDR();
}

static void I2C1_WriteStart(void)
{
    I2C1_Status.state = I2C_EVENT_SEND_WR_ADDR();
}

static void I2C1_Close(void)
{
    I2C1_Status.busy = false;
    I2C1_Status.address = 0xFF;
    I2C1_Status.writePtr = NULL;
    I2C1_Status.readPtr = NULL;
    I2C1_Status.state = I2C_STATE_IDLE;
    I2C1_ClearInterrupts();
    I2C1_DisableInterrupts();
    TWI1.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
}

static void I2C1_EventHandler(void)
{
    switch (I2C1_Status.state)
    {
        case I2C_STATE_IDLE:
            I2C1_Status.state = I2C_EVENT_IDLE();
            break;
        case I2C_STATE_SEND_RD_ADDR:
            I2C1_Status.state = I2C_EVENT_SEND_RD_ADDR();
            break;
        case I2C_STATE_SEND_WR_ADDR:
            I2C1_Status.state = I2C_EVENT_SEND_WR_ADDR();
            break;
        case I2C_STATE_TX:
            I2C1_Status.state = I2C_EVENT_TX();
            break;
        case I2C_STATE_RX:
            I2C1_Status.state = I2C_EVENT_RX();
            break;
        case I2C_STATE_NACK:
            I2C1_Status.state = I2C_EVENT_NACK();
            break;
        case I2C_STATE_ERROR:
            I2C1_Status.state = I2C_EVENT_ERROR();
            break;
        case I2C_STATE_STOP:
            I2C1_Status.state = I2C_EVENT_STOP();
            break;
        case I2C_STATE_RESET:
            I2C1_Status.state = I2C_EVENT_RESET();
            break;
    }

    //I2C1_Status.state = i2c1_eventTable[I2C1_Status.state]();
}

static void I2C1_ErrorEventHandler(void)
{
    if (I2C1_IsBusError())
    {
        I2C1_Status.state = I2C_STATE_ERROR;
        I2C1_Status.errorState = I2C_ERROR_BUS_COLLISION;
        TWI1.MSTATUS |= TWI_BUSERR_bm;
    }
    else if (I2C1_IsAddr() && I2C1_IsNack())
    {
        I2C1_Status.state = I2C_STATE_NACK;
        I2C1_Status.errorState = I2C_ERROR_ADDR_NACK;
        TWI1.MSTATUS |= TWI_RXACK_bm;
    }
    else if (I2C1_IsData() && I2C1_IsNack())
    {
        I2C1_Status.state = I2C_STATE_NACK;
        I2C1_Status.errorState = I2C_ERROR_DATA_NACK;
        TWI1.MSTATUS |= TWI_RXACK_bm;
    }
    else if(I2C1_IsArbitrationlostOverride())
    {
        I2C1_Status.state = I2C_STATE_ERROR;
        I2C1_Status.errorState = I2C_ERROR_BUS_COLLISION;
        TWI1.MSTATUS |= TWI_ARBLOST_bm;
    }
    I2C1_Status.state = i2c1_eventTable[I2C1_Status.state]();
    if(I2C1_Status.errorState != I2C_ERROR_NONE)
    {
        I2C1_Callback();
    }
}

static void I2C1_DefaultCallback(void)
{
    // Default Callback for Error Indication
}

/* I2C1 Event interfaces */
static i2c_event_states_t I2C_EVENT_IDLE(void)
{
    I2C1_Status.busy = false;
    TWI1.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
    return I2C_STATE_RESET;
}

static i2c_event_states_t I2C_EVENT_SEND_RD_ADDR(void)
{
    I2C1_SendTxAddr((uint8_t) (I2C1_Status.address << 1 | 1));
    return I2C_STATE_RX;
}

static i2c_event_states_t I2C_EVENT_SEND_WR_ADDR(void)
{
    I2C1_SendTxAddr((uint8_t) (I2C1_Status.address << 1));
    return I2C_STATE_TX;
}

static i2c_event_states_t I2C_EVENT_TX(void)
{
    i2c_event_states_t retEventState = I2C_STATE_TX;

        if (I2C1_Status.writeLength)
        {
            I2C1_Status.writeLength--;
            I2C1_SendTxData(*I2C1_Status.writePtr++);
            retEventState = I2C_STATE_TX;
        }
        else
        {
            if (I2C1_Status.switchToRead)
            {
                I2C1_Status.switchToRead = false;
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

    if (I2C1_Status.readLength == 1)
    {
        // Next byte will be last to be received, setup NACK
        I2C1_HostSendNack();
    }
    else
    {
        // More bytes to receive, setup ACK
        I2C1_HostSendAck();
    }

    if (--I2C1_Status.readLength)
    {
        *I2C1_Status.readPtr++ = I2C1_GetRxData();
        //Execute Acknowledge Action followed by a byte read operation
        TWI1.MCTRLB |= TWI_MCMD_RECVTRANS_gc;
        retEventState = I2C_STATE_RX;
    }
    else
    {
        *I2C1_Status.readPtr++ = I2C1_GetRxData();
        I2C1_HostSendNack();
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
    I2C1_ClearInterrupts();
    retEventState = I2C_EVENT_RESET();
    return retEventState;
}

static i2c_event_states_t I2C_EVENT_STOP(void)
{
    I2C1_SendStop();
    I2C1_Close();
    return I2C_STATE_IDLE;
}

static i2c_event_states_t I2C_EVENT_RESET(void)
{
    I2C1_ResetBus();
    I2C1_Status.busy = false;
    return I2C_STATE_IDLE;
}

/**
 Section: Register Level Interfaces
 */
static uint8_t I2C1_GetRxData(void)
{
    return TWI1.MDATA;
}

static void I2C1_SendTxData(uint8_t data)
{
    TWI1.MDATA = data;
}

static void I2C1_SendTxAddr(uint8_t data)
{
    TWI1.MADDR = data;
}

static inline void I2C1_ResetBus(void)
{
    //Set Clear Buffer Flag
    TWI1.MCTRLA &= ~(1 << TWI_ENABLE_bp);
    TWI1.MCTRLA |= 1 << TWI_ENABLE_bp;
    TWI1.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
}

static void I2C1_ClearInterruptFlag(void)
{
    if(TWI1.MSTATUS & TWI_RIF_bm)
    {
        TWI1.MSTATUS|= TWI_RIF_bm;
    }
    else if(TWI1.MSTATUS & TWI_WIF_bm)
    {
        TWI1.MSTATUS|= TWI_WIF_bm;
    }
}

static inline void I2C1_EnableRestart(void)
{
    //Enable I2C1 Restart
    TWI1.MCTRLB |= TWI_MCMD_REPSTART_gc;
}

static inline void I2C1_DisableRestart(void)
{
    //Disable I2C1 Restart
    TWI1.MCTRLB &= ~TWI_MCMD_REPSTART_gc;
}

static inline void I2C1_SendStop(void)
{
    //Clear Start Bit
    TWI1.MCTRLB |= TWI_MCMD_STOP_gc;
}

static bool I2C1_IsNack(void)
{
    return TWI1.MSTATUS & TWI_RXACK_bm;
}

static bool I2C1_IsBusError(void)
{
    return TWI1.MSTATUS & TWI_BUSERR_bm;
}

static bool I2C1_IsData(void)
{
    return TWI1.MDATA;
}

static bool I2C1_IsAddr(void)
{
    return TWI1.MADDR;
}

static bool I2C1_IsArbitrationlostOverride(void)
{
    return TWI1.MSTATUS & TWI_ARBLOST_bm;
}

static inline void I2C1_ClearErrorFlags(void)
{
   TWI1.MSTATUS |= TWI_RXACK_bm;
   TWI1.MSTATUS |= TWI_BUSERR_bm;
   TWI1.MSTATUS |= TWI_ARBLOST_bm;
}

static inline void I2C1_HostSendAck(void)
{
    TWI1.MCTRLB &= ~(1 << TWI_ACKACT_bp);
}

static inline void I2C1_HostSendNack(void)
{
    TWI1.MCTRLB |= TWI_ACKACT_NACK_gc;
}

static inline void I2C1_EnableInterrupts(void)
{
    TWI1.MCTRLA |= (TWI_RIEN_bm | TWI_WIEN_bm);
}

static inline void I2C1_DisableInterrupts(void)
{
    TWI1.MCTRLA &= ~(TWI_RIEN_bm | TWI_WIEN_bm);
}

static inline void I2C1_ClearInterrupts(void)
{
   TWI1.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);
}
