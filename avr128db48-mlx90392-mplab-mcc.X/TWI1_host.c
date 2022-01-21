#include "TWI1_host.h"

#include <avr/io.h>
#include <stdbool.h>

#define TWI_READ true
#define TWI_WRITE false

#define TWI1_IS_CLOCKHELD() TWI1.MSTATUS & TWI_CLKHOLD_bm
#define TWI1_IS_BUSERR() TWI1.MSTATUS & TWI_BUSERR_bm
#define TWI1_IS_ARBLOST() TWI1.MSTATUS & TWI_ARBLOST_bm

#define CLIENT1_NACK() TWI1.MSTATUS & TWI_RXACK_bm
#define CLIENT1_ACK() !(TWI1.MSTATUS & TWI_RXACK_bm)

#define TWI1_IS_BUSBUSY() (((TWI1.MSTATUS & TWI_BUSSTATE_gm) == TWI_BUSSTATE_BUSY_gc))
//#define TWI_IS_BAD() ((TWI_IS_BUSERR()) | (TWI_IS_ARBLOST()) | (CLIENT_NACK()) | (TWI_IS_BUSBUSY()))

#define TWI1_WAIT() while (!((TWI1_IS_CLOCKHELD()) || (TWI1_IS_BUSERR()) || (TWI1_IS_ARBLOST()) || (TWI1_IS_BUSBUSY())))

bool isTWI1Bad(void)
{
    //Checks for: NACK, ARBLOST, BUSERR, Bus Busy
    if ((((TWI1.MSTATUS) & (TWI_RXACK_bm | TWI_ARBLOST_bm | TWI_BUSERR_bm)) > 0)
            || (TWI1_IS_BUSBUSY()))
    {
        return true;
    }
    return false;
}

void TWI1_initHost(void)
{        
    //Standard 100kHz TWI, 4 Cycle Hold, 50ns SDA Hold Time
    TWI1.CTRLA = TWI_SDAHOLD_50NS_gc;    
    //Clear Dual Control
    TWI1.DUALCTRL = 0x00;
    
    //Enable Run in Debug
    TWI1.DBGCTRL = TWI_DBGRUN_bm;
    
    //Clear MSTATUS (write 1 to flags). BUSSTATE set to idle
    TWI1.MSTATUS = TWI_RIF_bm | TWI_WIF_bm | TWI_CLKHOLD_bm | TWI_RXACK_bm |
            TWI_ARBLOST_bm | TWI_BUSERR_bm | TWI_BUSSTATE_IDLE_gc;
    
    //Set for 100kHz from a 4MHz oscillator
    TWI1.MBAUD = 15;
    
    //[No ISRs] and Host Mode
    TWI1.MCTRLA = TWI_ENABLE_bm;
}

void TWI1_initPins(void)
{
    //PF2/PF3
        
    //Output I/O
    PORTF.DIRSET = PIN2_bm | PIN3_bm;

#ifdef TWI1_ENABLE_PULLUPS
    //Enable Pull-Ups
    PORTF.PINCONFIG = PORT_PULLUPEN_bm;
#endif

    //Select PF2/PF3
    PORTF.PINCTRLUPD = PIN2_bm | PIN3_bm;
}

bool _startTWI1(uint8_t addr, bool read)
{
    //If the Bus is Busy
    if (TWI1_IS_BUSBUSY())
    {
        return false;
    }
    
    //Send Address
    TWI1.MADDR = (addr << 1) | read;
    
    //Wait...
    TWI1_WAIT();
                
    if (isTWI1Bad())
    {
        //Stop the Bus
        TWI1.MCTRLB = TWI_MCMD_STOP_gc;
        return false;
    }
    
    //TWI Started
    return true;
}

//Internal Function: Reads len bytes from TWI, then issues a bus STOP
void _readFromTWI1(uint8_t* data, uint8_t len)
{
    uint8_t bCount = 0;
    
    //Release the clock hold
    
    TWI1.MSTATUS = TWI_CLKHOLD_bm;
    
    //TWI1.MCTRLB = TWI_MCMD_RECVTRANS_gc;
    
    while (bCount < len)
    {
        //Wait...
        TWI1_WAIT();
        
        //Store data
        data[bCount] = TWI1.MDATA;

        //Increment the counter
        bCount += 1;
        
        if (bCount != len)
        {
            //If not done, then ACK and read the next byte
            TWI1.MCTRLB = TWI_ACKACT_ACK_gc | TWI_MCMD_RECVTRANS_gc;
        }
    }
    
    //NACK and STOP the bus
    TWI1.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc;
}

//Internal Function: Write len bytes to TWI. Does NOT STOP the bus. Returns true if successful
bool _writeToTWI1(uint8_t* data, uint8_t len)
{
    uint8_t count = 0;
    
    while (count < len)
    {
        //Write a byte
        TWI1.MDATA = data[count];
        
        //Wait...
        TWI1_WAIT();
        
        //If the client NACKed, then abort the write
        if (CLIENT1_NACK())
        {
            return false;
        }
        
        //Increment the counter
        count++;
    }
    
    return true;
}

bool TWI1_sendByte(uint8_t addr, uint8_t data)
{
    //Address Client Device (Write)
    if (!_startTWI1(addr, TWI_WRITE))
        return false;
    
    bool success = _writeToTWI1(&data, 1);
    
    //Stop the bus
    TWI1.MCTRLB = TWI_MCMD_STOP_gc;
    
    return success;
}

bool TWI1_sendBytes(uint8_t addr, uint8_t* data, uint8_t len)
{
    //Address Client Device (Write)
    if (!_startTWI1(addr, TWI_WRITE))
        return false;
    
    //Write the bytes to the client
    bool success = _writeToTWI1(data, len);

    //Stop the bus
    TWI1.MCTRLB = TWI_MCMD_STOP_gc;
    
    
    
    return success;
}

bool TWI1_readByte(uint8_t addr, uint8_t* data)
{
    //Address Client Device (Read)
    if (!_startTWI1(addr, TWI_READ))
        return false;
    
    //Read byte from client
    _readFromTWI1(data, 1);

    return true;
}


bool TWI1_readBytes(uint8_t addr, uint8_t* data, uint8_t len)
{
    //Address Client Device (Read)
    if (!_startTWI1(addr, TWI_READ))
        return false;
    
    //Read bytes from client
    _readFromTWI1(data, len);
    
    return true;
}

bool TWI1_sendAndReadBytes(uint8_t addr, uint8_t regAddress, uint8_t* data, uint8_t len)
{
    //Address Client Device (Write)
    if (!_startTWI1(addr, TWI_WRITE))
        return false;
        
    //Write register address
    if (!_writeToTWI1(&regAddress, 1))
    {
        TWI1.MCTRLB = TWI_MCMD_STOP_gc;
        return false;
    }
    
    //Restart the TWI Bus in READ mode
    TWI1.MADDR |= TWI_READ;
    TWI1.MCTRLB = TWI_MCMD_REPSTART_gc;
    
    //Wait...
    TWI1_WAIT();
    
    if (isTWI1Bad())
    {
        //Stop the TWI Bus if an error occurred
        TWI1.MCTRLB = TWI_MCMD_STOP_gc;
        return false;
    }
    
    //Read the data from the client
    _readFromTWI1(data, len);
    
    return true;
}

bool TWI1_sendsAndReadBytes(uint8_t addr, uint8_t* write, uint8_t writeLen, uint8_t* read, uint8_t readLen)
{
    //Address Client Device (Write)
    if (!_startTWI1(addr, TWI_WRITE))
        return false;
        
    //Write register address
    if (!_writeToTWI1(write, writeLen))
    {
        TWI1.MCTRLB = TWI_MCMD_STOP_gc;
        return false;
    }
    
    //Restart the TWI Bus in READ mode
    TWI1.MADDR |= TWI_READ;
    TWI1.MCTRLB = TWI_MCMD_REPSTART_gc;
    
    //Wait...
    TWI1_WAIT();
    
    if (isTWI1Bad())
    {
        //Stop the TWI Bus if an error occurred
        TWI1.MCTRLB = TWI_MCMD_STOP_gc;
        return false;
    }
    
    //Read the data from the client
    _readFromTWI1(read, readLen);
    
    return true;

}