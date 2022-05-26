#include "TWI0_host.h"
#include "MVIO.h"
#include "TCB0_oneShot.h"

#include <avr/io.h>
#include <stdbool.h>

#define TWI_READ true
#define TWI_WRITE false

//10ms timeout
#define TWI0_TIMEOUT 10

#define TWI0_IS_CLOCKHELD() TWI0.MSTATUS & TWI_CLKHOLD_bm
#define TWI0_IS_BUSERR() TWI0.MSTATUS & TWI_BUSERR_bm
#define TWI0_IS_ARBLOST() TWI0.MSTATUS & TWI_ARBLOST_bm

#define CLIENT0_NACK() TWI0.MSTATUS & TWI_RXACK_bm
#define CLIENT0_ACK() !(TWI0.MSTATUS & TWI_RXACK_bm)

#define TWI0_IS_BUSBUSY() ((((TWI0.MSTATUS & TWI_BUSSTATE_gm) == TWI_BUSSTATE_BUSY_gc)))
//#define TWI_IS_BAD() ((TWI_IS_BUSERR()) | (TWI_IS_ARBLOST()) | (CLIENT_NACK()) | (TWI_IS_BUSBUSY()))

//Timer, MVIO, else...
//#define TWI0_WAIT() while (!((TWI0_IS_CLOCKHELD()) || (TWI0_IS_BUSERR()) || (TWI0_IS_ARBLOST()) || (TWI0_IS_BUSBUSY())))

bool TWI0_Wait(void)
{
    //Start the 1ms timer
    TCB0_triggerTimer();
    uint8_t timerCounter = 0;
    
    do
    {
        if (!MVIO_isOK())
        {
            //MVIO Failure
            return false;
        }
        
        if (!TCB0_isRunning())
        {
            //Need to start the timer
            timerCounter++;
            TCB0_triggerTimer();
        }
        
        if (timerCounter == TWI0_TIMEOUT)
        {
            return false;
        }
        
    } while (!((TWI0_IS_CLOCKHELD()) || (TWI0_IS_BUSERR()) || (TWI0_IS_ARBLOST()) || (TWI0_IS_BUSBUSY())));
    return true;
}

bool isTWI0Bad(void)
{
    //If MVIO is not active...
    if (!MVIO_isOK())
    {
        return true;
    }
    
    //Checks for: NACK, ARBLOST, BUSERR, Bus Busy
    if ((((TWI0.MSTATUS) & (TWI_RXACK_bm | TWI_ARBLOST_bm | TWI_BUSERR_bm)) > 0)
            || (TWI0_IS_BUSBUSY()))
    {
        return true;
    }
    return false;
}

void TWI0_initHost(void)
{        
    //Standard 100kHz TWI, 4 Cycle Hold, 50ns SDA Hold Time
    TWI0.CTRLA = TWI_SDAHOLD_50NS_gc;   
    
    //Clear Dual Control
    TWI0.DUALCTRL = 0x00;
    
    //Enable Run in Debug
    TWI0.DBGCTRL = TWI_DBGRUN_bm;
    
    //Clear MSTATUS (write 1 to flags). BUSSTATE set to idle
    TWI0.MSTATUS = TWI_RIF_bm | TWI_WIF_bm | TWI_CLKHOLD_bm | TWI_RXACK_bm |
            TWI_ARBLOST_bm | TWI_BUSERR_bm | TWI_BUSSTATE_IDLE_gc;
    
    //Set for 100kHz from a 4MHz oscillator
    TWI0.MBAUD = 15;
    
    //[No ISRs] and Host Mode
    TWI0.MCTRLA = TWI_ENABLE_bm;
}

void TWI0_initPins(void)
{
    //PC2/PC3
        
    //Output I/O
    PORTC.DIRSET = PIN2_bm | PIN3_bm;
    PORTC.OUTCLR = PIN2_bm | PIN3_bm;

#ifdef TWI0_ENABLE_PULLUPS
    //Enable Pull-Ups
    PORTC.PINCONFIG = PORT_PULLUPEN_bm;
    
    //Select PC2/PC3
    PORTC.PINCTRLUPD = PIN2_bm | PIN3_bm;
#endif
    
    //Set PORTMUX
    PORTMUX.TWIROUTEA &= ~(PORTMUX_TWI0_gm);
    PORTMUX.TWIROUTEA |= PORTMUX_TWI0_ALT2_gc;
}

void TWI0_flush(void)
{
    //Flush and Reset
    //TWI0.MCTRLB |= TWI_FLUSH_bm;
}

bool _startTWI0(uint8_t addr, bool read)
{
    //If the bus is not high on both pins
//    if (!((VPORTC.IN & PIN2_bm) && (VPORTC.IN & PIN3_bm)))
//    {
//        return false;
//    }
    
    //If the Bus is Busy
    if (TWI0_IS_BUSBUSY())
    {
        return false;
    }
    
    //Send Address
    TWI0.MADDR = (addr << 1) | read;
    
    //Wait...
    if (!TWI0_Wait())
    {
        //Something went wrong
        //Stop the TWI Bus if an error occurred
        TWI0.MCTRLB = TWI_MCMD_STOP_gc;
        return false;
    }
                
    if (isTWI0Bad())
    {
        //Stop the Bus
        TWI0.MCTRLB = TWI_MCMD_STOP_gc;
        return false;
    }
    
    //TWI Started
    return true;
}

//Internal Function: Reads len bytes from TWI, then issues a bus STOP
void _readFromTWI0(uint8_t* data, uint8_t len)
{
    uint8_t bCount = 0;
    
    //Release the clock hold
    
    TWI0.MSTATUS = TWI_CLKHOLD_bm;
    
    //TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;
    
    while (bCount < len)
    {
        //Wait...
        if (!TWI0_Wait())
        {
            //Something went wrong
            //Stop the TWI Bus if an error occurred
            TWI0.MCTRLB = TWI_MCMD_STOP_gc;
            return;
        }
        
        //Store data
        data[bCount] = TWI0.MDATA;

        //Increment the counter
        bCount += 1;
        
        if (bCount != len)
        {
            //If not done, then ACK and read the next byte
            TWI0.MCTRLB = TWI_ACKACT_ACK_gc | TWI_MCMD_RECVTRANS_gc;
        }
    }
    
    //NACK and STOP the bus
    TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc;
}

//Internal Function: Write len bytes to TWI. Does NOT STOP the bus. Returns true if successful
bool _writeToTWI0(uint8_t* data, uint8_t len)
{
    uint8_t count = 0;
    
    while (count < len)
    {
        //Write a byte
        TWI0.MDATA = data[count];
        
        //Wait...
        if (!TWI0_Wait())
        {
            //Something went wrong
            //Stop the TWI Bus if an error occurred
            return false;
        }

        
        //If the client NACKed, then abort the write
        if (CLIENT0_NACK())
        {
            return false;
        }
        
        //Increment the counter
        count++;
    }
    
    return true;
}

bool TWI0_sendByte(uint8_t addr, uint8_t data)
{
    //Address Client Device (Write)
    if (!_startTWI0(addr, TWI_WRITE))
        return false;
    
    bool success = _writeToTWI0(&data, 1);
    
    //Stop the bus
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
    return success;
}

bool TWI0_sendBytes(uint8_t addr, uint8_t* data, uint8_t len)
{
    //Address Client Device (Write)
    if (!_startTWI0(addr, TWI_WRITE))
        return false;
    
    //Write the bytes to the client
    bool success = _writeToTWI0(data, len);

    //Stop the bus
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
    
    
    return success;
}

bool TWI0_readByte(uint8_t addr, uint8_t* data)
{
    //Address Client Device (Read)
    if (!_startTWI0(addr, TWI_READ))
        return false;
    
    //Read byte from client
    _readFromTWI0(data, 1);

    return true;
}


bool TWI0_readBytes(uint8_t addr, uint8_t* data, uint8_t len)
{
    //Address Client Device (Read)
    if (!_startTWI0(addr, TWI_READ))
        return false;
    
    //Read bytes from client
    _readFromTWI0(data, len);
    
    return true;
}

bool TWI0_sendAndReadBytes(uint8_t addr, uint8_t regAddress, uint8_t* data, uint8_t len)
{
    //Address Client Device (Write)
    if (!_startTWI0(addr, TWI_WRITE))
        return false;
        
    //Write register address
    if (!_writeToTWI0(&regAddress, 1))
    {
        TWI0.MCTRLB = TWI_MCMD_STOP_gc;
        return false;
    }
    
    //Restart the TWI Bus in READ mode
    TWI0.MADDR |= TWI_READ;
    TWI0.MCTRLB = TWI_MCMD_REPSTART_gc;
    
    //Wait...
    if (!TWI0_Wait())
    {
        //Something went wrong
        //Stop the TWI Bus if an error occurred
        TWI0.MCTRLB = TWI_MCMD_STOP_gc;
        return false;
    }
    
    if (isTWI0Bad())
    {
        //Stop the TWI Bus if an error occurred
        TWI0.MCTRLB = TWI_MCMD_STOP_gc;
        return false;
    }
    
    //Read the data from the client
    _readFromTWI0(data, len);
    
    return true;
}

bool TWI0_sendsAndReadBytes(uint8_t addr, uint8_t* write, uint8_t writeLen, uint8_t* read, uint8_t readLen)
{
    //Address Client Device (Write)
    if (!_startTWI0(addr, TWI_WRITE))
        return false;
        
    //Write register address
    if (!_writeToTWI0(write, writeLen))
    {
        TWI0.MCTRLB = TWI_MCMD_STOP_gc;
        return false;
    }
    
    //Restart the TWI Bus in READ mode
    TWI0.MADDR |= TWI_READ;
    TWI0.MCTRLB = TWI_MCMD_REPSTART_gc;
    
    //Wait...
    TWI0_Wait();
    
    if (isTWI0Bad())
    {
        //Stop the TWI Bus if an error occurred
        TWI0.MCTRLB = TWI_MCMD_STOP_gc;
        return false;
    }
    
    //Read the data from the client
    _readFromTWI0(read, readLen);
    
    return true;

}