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
#include "Serial.h"
#include "LED_RED.h"
#include "LED_GREEN.h"
//#include "LED_BLUE.h"
#include "AS1.h"
#include "WAIT1.h"
#include "PWM1.h"

/* local prototypes (static functions) */
static void APP_HandleEvent(EVNT_Handle event);

uint16_t ledB;

void APP_Init(void) {
    EVNT_Init();
}

void APP_Loop(void) {
	EVNT_SetEvent(EVNT_INIT);
	LED_GREEN_Put(0);
	//LED_BLUE_SetRatio16(32768);
	//Inhr1_SetRatio16(32768);
	PWM1_SetRatio16(0);
	
	
    while(1) {
        // Task 1: Handle Events
        EVNT_HandleEvent(APP_HandleEvent);

        // Further Tasks...

        // Down here testing PWM only...
        //Inhr1_SetRatio16(32768);
        /*
        LED_BLUE_SetRatio16(1000);
        WAIT1_Waitms(100);
        LED_BLUE_SetRatio16(10000);
        WAIT1_Waitms(100);
        LED_BLUE_SetRatio16(20000);
        WAIT1_Waitms(100);
        LED_BLUE_SetRatio16(35000);
        WAIT1_Waitms(100);
        LED_BLUE_SetRatio16(50000);
        WAIT1_Waitms(100);
        LED_BLUE_SetRatio16(65000);
        WAIT1_Waitms(100);
        */
        /*
        LED_BLUE_SetRatio16(ledB);
        ledB += 100;
        WAIT1_Waitms(10);
        */
        // Testing only...
        //EVNT_SetEvent(EVNT_INIT);
        //LED_GREEN_Put(1);
        //WAIT1_Waitms(200);
        //LED_GREEN_Put(0);
        //WAIT1_Waitms(200);

    }
}

static void APP_HandleEvent(EVNT_Handle event) {
    switch(event) {
        case EVNT_INIT: 
        	AS1_SendChar('E');
        	ledB = 0;
            /* todo: fill events */
            break;
            
        case EVNT_HEARTBEAT:
        	
        	//ledB += 100;
        	//PWM1_SetRatio16(ledB);
        	
        	//LED_BLUE_Neg();
        	break;

        case EVNT_SERIAL_CMD:
        	switch(*SER_GetCommand()) {
                case SER_START:
                    break;

                case SER_END:
                    break;

                case SER_MODE:
                    break;

                case SER_BLOCK_ARRAY:
                    break;

                case SER_RUN:
                    break;

                case SER_PICK_BLOCK:
                    break;

                case SER_RELEASE_BLOCK:
                    break;

                case SER_SET_POSITION:
                    break;

                case SER_GET_POSITION:
                    break;

                case SER_GET_VERSION:
                    break;

                case SER_ZERO_POSITION:
                    break;

                default:
                    break;
        	}

            AS1_SendChar(*SER_GetCommand());
            AS1_SendChar(SER_GetData()[0]);
            AS1_SendChar(SER_GetData()[1]);



        	SER_SetHandled();
            
        	LED_GREEN_Neg();
        	break;
        	
        default:
            break;
    }
}
