/**
 * \file
 * \brief Main Application Module.
 * \author Christoph Bächler
 * \date 11.11.2013
 *
 * This file contains the main application code for PREN T15 robot.
 */

#include "PE_Types.h"
#include "Application.h"
#include "Event.h"
#include "Trigger.h"
#include "LED_RED.h"
#include "LED_GREEN.h"
#include "LED_BLUE.h"
#include "AS1.h"
#include "WAIT1.h"

/* local prototypes (static functions) */
static void APP_HandleEvent(EVNT_Handle event);

void APP_Init(void) {
    EVNT_Init();
}

void APP_Loop(void) {
	EVNT_SetEvent(EVNT_INIT);
	LED_GREEN_Put(0);
    while(1) {
        // Task 1: Handle Events
        EVNT_HandleEvent(APP_HandleEvent);

        // Testing only...
        //EVNT_SetEvent(EVNT_INIT);
        //LED_GREEN_Put(1);
        //WAIT1_Waitms(200);
        //LED_GREEN_Put(0);
        //WAIT1_Waitms(200);
        // Further Tasks...

    }
}

static void APP_HandleEvent(EVNT_Handle event) {
    switch(event) {
        case EVNT_INIT: 
        	AS1_SendChar('E');
            /* todo: fill events */
            break;
            
        case EVNT_HEARTBEAT:
        	LED_BLUE_Neg();
        	break;

        default:
            break;
    }
}
