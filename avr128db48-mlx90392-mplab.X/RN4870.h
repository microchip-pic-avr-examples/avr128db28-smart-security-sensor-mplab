#ifndef RN4870_H
#define	RN4870_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
      
//If set, BLE module is expected to be powered up on init.
//#define RN4870_NO_POWER_GATE_TEST 
        
    typedef enum {
        RN4870_EVENT_NONE = 0, RN4870_EVENT_REBOOT, RN4870_EVENT_STREAM_OPEN, RN4870_EVENT_CONN_PARAM,
                RN4870_EVENT_CONNECT, RN4870_EVENT_DISCONNECT, RN4870_EVENT_BONDED
    } RN4870_EVENT;
    
    //Configures the RN4870
    void RN4870_init(void);
        
    //Setup on initial power-up
    bool RN4870_startupInit(void);
        
    //Sets a user event handler
    void RN4870_setUserEventHandler(bool (*userEvent)(void));
    
    //Returns true if the module is not active, and the MCU can enter sleep
    bool RN4870_canSleep(void);
    
    //Handles user commands and status messages
    void RN4870_processEvents(void);
    
    //Processes user commands from the RN4870 (called by RN4870_processEvents)
    void RN4870_runUserCommands(void);

    //Handles status messages from the RN4870 (called by RN4870_processEvents)
    void RN4870_processStatusMessages(void);
    
    //Returns a status event, if one is available
    RN4870_EVENT RN4870_getStatusEvent(void);
    
    //Powers up the RN4870. Non-Blocking
    void RN4870_powerUp(void);
    
    //Powers down the RN4870. Non-Blocking
    void RN4870_powerDown(void);
    
    //Enters Command Mode in the RN4870
    bool RN4870_enterCommandMode(void);
    
    //Exits Command Mode in the RN4870
    void RN4870_exitCommandMode(void);
    
    //Returns true if connected
    bool RN4870_isConnected(uint8_t timeout);
    
    //Sends a Command to the RN4870
    bool RN4870_sendCommand(const char* string, uint8_t timeout);
    
    //Sends a Command to the RN4870, and prints a DBG log
    void RN4870_sendCommandAndPrint(const char* string, uint8_t timeout);
    
    //Returns true if ready to send data
    bool RN4870_isReady(void);
    
    //Sends a string to the user, if powered up.
    void RN4870_sendStringToUser(const char* str);

    //Returns the char buffer associated with the BLE
    char* RN4870_getCharBuffer(void);
    
    //Sends the cached string to the user, if powered up
    void RN4870_printBufferedString(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* RN4870_H */

