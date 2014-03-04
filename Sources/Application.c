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
#include "Database.h"
#include "Robot.h"
#include "Event.h"
#include "Motors.h"
#include "Trigger.h"
#include "BlockStack.h"
#include "Serial.h"
#include "DBG.h"
#include "WAIT.h"
#include "M1_MODE0.h"
#include "M1_MODE1.h"
#include "M1_MODE2.h"

#include "LED_RED.h"
#include "LED_GREEN.h"
#include "LED_BLUE.h"

/* local prototypes (static functions) */
static void APP_HandleEvent(EVNT_Handle event);
static void APP_Blink(void *p);

/*! \brief Application initialisation routine.
 *
 * This function does the initialisation of hardware and data structures. 
 */
void APP_Init(void) {
	DB_Init();
    EVNT_Init();
    TRG_Init();
    SER_Init();
    MOT_Init();
}

/*! \brief Application main loop.
 *
 * This is the main loop of the application. Be sure to implement a 
 * while(1) loop here, else the function will be left and deinitialisation 
 * of the components will be started in ProcessorExpert.c. 
 */
void APP_Loop(void) {
	LED_GREEN_On();
	lift.position = 500;
	LED_GREEN_Off();
	
	EVNT_SetEvent(EVNT_INIT);
			
    while(1) {
        // Task 1: Handle Events
        EVNT_HandleEvent(APP_HandleEvent);

        // Task 2: Handle Picking 
        ROB_Run();
        
        // Further Tasks...
    }
}

static void APP_Blink(void *p) {
	LED_GREEN_Neg();
	TRG_SetTrigger(TRG_LED_BLINK, 500, APP_Blink, NULL);
}

/*! \brief Event handler routine.
 *
 * This is implemented as described in INTRO script by Erich Styger. Basically 
 * all events that can be generated from every part of the software will be 
 * handled here. 
 *
 *  \param event  Event handle.
 */
static void APP_HandleEvent(EVNT_Handle event) {
	uint16_t accel; 
	uint16_t decel; 
	uint16_t speed;
	BLOCK_Object block;
	
    switch(event) {
        case EVNT_INIT: 
			LED_BLUE_On();
			WAIT_Waitms(500);
			LED_BLUE_Off();
			TRG_SetTrigger(TRG_LED_BLINK, 500, APP_Blink, NULL);
            break;
            
        case EVNT_HEARTBEAT:
        	LED_BLUE_Neg();
        	break;

        case EVNT_SERIAL_CMD:
        	switch(*SER_GetCommand()) {	
                case '1':
                    LED_RED_On();
                    DB_SaveNVM();
                    SER_SendPacket('1');
                    break;

                case '2':
                    LED_RED_Off();
                    DB_SaveNVM();
                    SER_SendPacket('2');
                    break;
                    
        		case 'P':
        			SER_AddData16(MOT_Process(&rotary));
        			SER_AddData16(MOT_Process(&knee));
        			SER_AddData16(MOT_Process(&lift));
        			SER_SendPacket('D');
        			
        			break;
        			
        		case 'Q':
        			//MOT_MoveSteps(&rotary, SER_GetData16(0));
        			//MOT_MoveSteps(&knee, SER_GetData16(2));
        			MOT_MoveSteps(&lift,   (int16_t) (SER_GetData16(4)-lift.position));
        			//MOT_MoveSteps(&lift, SER_GetData16(4));
        			SER_SendPacket('Q');
        			break;
        			
        		case 'm': 
        			accel = SER_GetData16(1);
					decel = SER_GetData16(3);
					speed = SER_GetData16(5);
        			        			
        			switch(SER_GetData8(0)) {
						case '0':
							MOT_CalcValues(&rotary, accel, decel, speed);
							SER_SendPacket('m');
							break;
							
						case '1':
							MOT_CalcValues(&knee, accel, decel, speed);
							SER_SendPacket('m');
							break;
							
						case '2':
							MOT_CalcValues(&lift, accel, decel, speed);
							SER_SendPacket('m');
							break;
							
						default: 
							break;
        			}
        			break;
					
				case 'd': 
					//SER_AddData16(rotary.step_count);
					//SER_AddData16(rotary.step_delay);
					//SER_AddData16((uint16_t) ROB_GetStateArray());
					SER_AddData16((uint16_t) MOT_GetState(&rotary));
					SER_AddData16((uint16_t) MOT_GetState(&knee));
					SER_AddData16((uint16_t) MOT_GetState(&lift));
					
					SER_AddData16(rotary.position);
					SER_AddData16(knee.position);
                    SER_AddData16(lift.position);                    
					SER_AddData16((uint16_t) BLOCK_GetSize());
					//SER_AddData16((uint16_t) ROB_GetRunMode());
					SER_SendPacket('d');
					break;
        				
					
				case 'a':	// push 
					MOT_SetStepMode(&rotary, MOT_STEP_2);
					block.x = SER_GetData16(0);
					block.y = SER_GetData16(2);
					BLOCK_Push(block);
					//BLOCK_Push(SER_GetData8(0));
                    //SER_AddData8(*SER_GetLength());
					SER_SendPacket('a');
					break;
					
				case 'b': 	// pop
					MOT_SetStepMode(&rotary, MOT_STEP_16);
					block = BLOCK_Pop();
					SER_AddData16(block.x);
					SER_AddData16(block.y);
					
					//SER_AddData8(BLOCK_Pop());
					SER_SendPacket('b');
					break;
					
                case SER_MODE:
                    ROB_SetRunMode(SER_GetData8(0));
                    break;

                case SER_RUN:
                	ROB_SetRunMode(ROB_PICKPLACE);
                	SER_SendPacket(SER_RUN);
                    //ROB_Run();
                    break;

                case SER_BLOCK_ARRAY:
                	/*
                    pack = ((*SER_GetLength())-5)/sizeof(BLOCK_Object);
                    for(i=0; i<pack; i++) {
                    	BLOCK_Push(SER_GetData8(i));
                    }
                    SER_SendPacket(SER_BLOCK_ARRAY);
                	*/
/*
                	use size_of(BLOCK_Object) here.
                	(get length - constant) modulo num_bytes_per_block
                	for loop, for each block
                		get alpha, beta from serial packet
                		add block to list
                	next block
                	send answer packet   */             	
                    break;

                
/*
                case SER_PICK_BLOCK:
                	enable vaccuum
                    break;

                case SER_RELEASE_BLOCK:
                	disable vaccuum
                    break;

                case SER_SET_POSITION:
                	read new position from serial packet
                	set the steps
                	wait for end of move
                	send answer packet
                    break;

*/
                case SER_GET_POSITION:
                	//return current position of the arm (alpha, beta, delta)
                	SER_AddData16(rotary.position);
                	SER_AddData16(knee.position);
                	SER_SendPacket(SER_GET_POSITION);
                    break;
                    
                case SER_READ_VARIABLE: 
                	SER_AddData8(SER_GetData8(0));
                	switch(DB_GetType(SER_GetData8(0))) {
						case U8: {
							SER_AddData8(*((uint8_t*) DB_GetVar(SER_GetData8(0))));
							break;
						}
						case U16: {
							SER_AddData16(*((uint16_t*) DB_GetVar(SER_GetData8(0))));
							break;
						}
						case MOT: {
                            MOT_PubData* t = (MOT_PubData*) DB_GetVar(SER_GetData8(0));
							SER_AddData16(t->accel);
                            SER_AddData16(t->decel);
                            SER_AddData16(t->speed);
                            break;
						}
						case T_DBGBUFFER: {
							break;
						}
					}
					
					SER_SendPacket(SER_WRITE_VARIABLE);
                	break;
                	
                case SER_WRITE_VARIABLE: 
                	switch(DB_GetType(SER_GetData8(0))) {
                		case U8: {
                			(*(uint8_t*) DB_GetVar(SER_GetData8(0))) = SER_GetData8(1);
                			break;
                		}
                		case U16: {
							(*(uint16_t*) DB_GetVar(SER_GetData8(0))) = SER_GetData16(1);
							break;
						}
                		case MOT: {
                			((MOT_PubData*) DB_GetVar(SER_GetData8(0)))->accel = SER_GetData16(1);	
                			((MOT_PubData*) DB_GetVar(SER_GetData8(0)))->decel = SER_GetData16(3);
                			((MOT_PubData*) DB_GetVar(SER_GetData8(0)))->speed = SER_GetData16(5);
							break;
						}
                		case T_DBGBUFFER: {
                			break;
                		}
                	}

                	SER_SendPacket(SER_WRITE_VARIABLE);            	
                	break;
                	
				case SER_SAVE_NVM:
					DB_SaveNVM();
					SER_SendPacket(SER_SAVE_NVM);
					break;
                    
                    
/*
                case SER_GET_VERSION:
                	return the current version of the firmware
                    break;

                case SER_ZERO_POSITION:
                	slowly drive to the limits
                	set null-pos in motor registers
                	send answer packet
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
