#ifndef WINDOWALARM_MESSAGES_H
#define	WINDOWALARM_MESSAGES_H

#ifdef	__cplusplus
extern "C" {
#endif

    const char* calInstructions[] = { 
        "", //CAL BAD
        "", //CAL INIT
        "Open the window and press the button to start.", //CAL OPEN WAIT
        "", //CAL OPEN
        "Please close the window and the press the button to continue.", //CAL CLOSED WAIT
        "", //CAL CLOSED
        "Please open the window to the desired threshold, then press the button.", //CAL CRACKED WAIT
        "[ERR] Window is too open. Please close the window and press the button to retry.", //CAL CRACKED ERR
        "", //CAL CRACKED,
        "Please close the window and press the button to finish." //CAL CLOSED FINAL WAIT
        "", //CAL DEINIT
        "", //CAL GOOD
        ""
    };
    
#ifdef	__cplusplus
}
#endif

#endif	/* WINDOWALARM_MESSAGES_H */

