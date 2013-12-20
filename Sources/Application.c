/**
 * \file
 * \brief Main Application Module.
 * \author Christoph Bächler
 * \date 11.11.2013
 *
 * This file contains the main application code for PREN T15 robot. On 
 * startup of the microcontroller the \ref APP_Init() and \ref APP_Loop() 
 * are called from ProcessorExpert.c. 
 * Here also the Event Handler is implemented which does different actions 
 * depending on the generated event. Every part of the software can be 
 * the source of an event. 
 */

#include "PE_Types.h"
#include "Application.h"
#include "Event.h"
#include "Motors.h"
#include "Trigger.h"
#include "Serial.h"
#include "AS1.h"
#include "WAIT1.h"

#include "LED_RED.h"
#include "LED_GREEN.h"
#include "LED_BLUE.h"

#include "M1.h"
#include "M2.h"

/* local prototypes (static functions) */
static void APP_HandleEvent(EVNT_Handle event);

/*! \brief Application initialisation routine.
 *
 * This function does the initialisation of hardware and data structures. 
 */
void APP_Init(void) {
    EVNT_Init();
    MOT_Init();
}

/*! \brief Application main loop.
 *
 * This is the main loop of the application. Be sure to implement a 
 * while(1) loop here, else the function will be left and deinitialisation 
 * of the components will be started in ProcessorExpert.c. 
 */
void APP_Loop(void) {
	//EVNT_SetEvent(EVNT_INIT);
	
	LED_GREEN_On();
	MOT_CalcValues(&rotary, 2000, 2000, 800);
	MOT_CalcValues(&knee, 2000, 2000, 500);
	MOT_CalcValues(&lift, 250, 250, 800);
	LED_GREEN_Off();
	
    while(1) {
        // Task 1: Handle Events
        EVNT_HandleEvent(APP_HandleEvent);

        // Further Tasks...
    }
}

/*! \brief Event handler routine.
 *
 * This is implemented as described in INTRO script by Erich Styger. Basicly 
 * all events that can be generated from every part of the software will be 
 * handled here. 
 *
 *  \param event  Event handle.
 */
static void APP_HandleEvent(EVNT_Handle event) {
	uint16_t val;
	uint16_t steps;
	
    switch(event) {
        case EVNT_INIT: 
        	LED_RED_On();
        	WAIT1_Waitms(200);
        	LED_RED_Off();
            break;
            
        case EVNT_HEARTBEAT:
        	LED_BLUE_Neg();
        	break;

        case EVNT_SERIAL_CMD:
        	switch(*SER_GetCommand()) {					
        		case 'P':
        			//SER_AddData16(0x1234);
        			//SER_SendPacket('P');
        			SER_AddData16(MOT_Process(&rotary));
        			SER_AddData16(MOT_Process(&knee));
        			SER_AddData16(MOT_Process(&lift));
        			SER_SendPacket('D');
        			
        			break;
        			
        		case 'Q':
        			steps = SER_GetData16(0);	//(SER_GetData()[0]<<8)+SER_GetData()[1];
        		    //accel = SER_GetData16(2);	//(SER_GetData()[2]<<8)+SER_GetData()[3];
        			//decel = SER_GetData16(4);	//(SER_GetData()[4]<<8)+SER_GetData()[5];
        			//speed = SER_GetData16(6);	//(SER_GetData()[6]<<8)+SER_GetData()[7];
        			
        			// recalculate motor values based on accel, decel and speed
        			
        			MOT_MoveSteps(&rotary, steps);
        			MOT_MoveSteps(&knee, steps);
        			MOT_MoveSteps(&lift, steps);
        			
        			SER_SendPacket('Q');
        			break;
					
        			/*
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
*/
                default:
                	// send error message
                    break;
        	}

        	SER_SetHandled();
        	break;
        	
        default:
            break;
    }
}
