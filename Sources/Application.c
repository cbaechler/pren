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
#include "Motors.h"
#include "Trigger.h"
#include "Serial.h"
#include "AS1.h"
#include "WAIT1.h"
#include "PWMred.h"
#include "PWMgreen.h"
#include "PWMblue.h"

/* local prototypes (static functions) */
static void APP_HandleEvent(EVNT_Handle event);

uint16_t ledB;

void APP_Init(void) {
    EVNT_Init();
    MOT_Init();
}

void APP_Loop(void) {
	uint16_t i;
	EVNT_SetEvent(EVNT_INIT);
	//LED_GREEN_Put(0);
	//LED_BLUE_SetRatio16(32768);
	//Inhr1_SetRatio16(32768);
	PWMred_SetRatio16(0);
	PWMgreen_SetRatio16(0);
	PWMblue_SetRatio16(0);
	
	speed_cntr_Move(1000, 500, 500, 800);
	/*
	for(i = 0; i<1000; i++) {
		speed_cntr_TIMER1_COMPA_interrupt();
		WAIT1_Waitms(10);
	}
	*/
	//AS1_SendChar(sizeof(int));
	//AS1_SendChar(sizeof(long));
	
	
	
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
	uint16_t val;
    switch(event) {
        case EVNT_INIT: 
        	//AS1_SendChar('E');
        	//ledB = 0;
            /* todo: fill events */
            break;
            
        case EVNT_HEARTBEAT:
        	
        	//ledB += 100;
        	//PWM1_SetRatio16(ledB);
        	
        	//LED_BLUE_Neg();
        	break;

        case EVNT_SERIAL_CMD:
        	switch(*SER_GetCommand()) {
        		case 'r': 
        			val = (SER_GetData()[0]<<8)+SER_GetData()[1];
        			PWMred_SetRatio16(val);		
        			break;
        			
        		case 'g': 
					val = (SER_GetData()[0]<<8)+SER_GetData()[1];
					PWMgreen_SetRatio16(val);  			
					break;
        			
        		case 'b': 
					val = (SER_GetData()[0]<<8)+SER_GetData()[1];
					PWMblue_SetRatio16(val);         			
					break;
					
        		case 'P':
        			//SER_AddData16(0x1234);
        			//SER_SendPacket('P');
        			speed_cntr_TIMER1_COMPA_interrupt();
        			break;
        			
        		case 'Q': 
        			speed_cntr_Move(((SER_GetData()[0]<<8)+SER_GetData()[1]), 
        					((SER_GetData()[2]<<8)+SER_GetData()[3]), 
        					((SER_GetData()[4]<<8)+SER_GetData()[5]), 
        					((SER_GetData()[6]<<8)+SER_GetData()[7]));
        			SER_SendPacket('Q');
        			break;
					
        			/*
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
*/
                default:
                    break;
        	}

            //AS1_SendChar(*SER_GetCommand());
            //AS1_SendChar(SER_GetData()[0]);
            //AS1_SendChar(SER_GetData()[1]);
            
            //SER_AddData8(0x44);
            //SER_AddData8(0x55);
            //SER_AddData16(0x1234);
            //SER_SendPacket(0x27);

            //SER_SendPacket();
        	SER_SetHandled();
            
        	//LED_GREEN_Neg();
        	break;
        	
        default:
            break;
    }
}
