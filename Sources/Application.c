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
#include "WAIT.h"

#include "LED_S1.h"
#include "LED_S2.h"
#include "LED_ER.h"
#include "LED_RED.h"
#include "LED_GREEN.h"
#include "LED_BLUE.h"
#include "SW1.h"
#include "VALVE.h"

/* local prototypes (static functions) */
static void APP_HandleEvent(EVNT_Handle event);
static void APP_Blink(void *p);
static void APP_KeyPoll(void *p);
static void APP_BlueLedOff(void *p);

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
    ROB_Init();
}

/*! \brief Application main loop.
 *
 * This is the main loop of the application. Be sure to implement a 
 * while(1) loop here, else the function will be left and deinitialisation 
 * of the components will be started in ProcessorExpert.c. 
 */
void APP_Loop(void) {
	lift.position = 0;
	LED_S2_On();
	EVNT_SetEvent(EVNT_INIT);
	
    while(1) {
        // Task 1: Handle Events
        EVNT_HandleEvent(APP_HandleEvent);

        // Task 2: Handle Picking 
        ROB_Process();
        
        // Further Tasks...
    }
}

static void APP_Blink(void *p) {
	LED_S1_Neg();
	LED_S2_Neg();
	LED_ER_Neg();
	TRG_SetTrigger(TRG_LED_BLINK, 1000, APP_Blink, NULL);
}

static void APP_KeyPoll(void *p) {
	static uint8_t debounce_cnt;

	if(!SW1_GetVal()) {
		debounce_cnt++;
		if(debounce_cnt >= 100) {
			EVNT_SetEvent(EVNT_SAVE_NVM);
			debounce_cnt = 0;
		}
	}
	else {
		debounce_cnt = 0;
	}
	
	TRG_SetTrigger(TRG_KEY_POLL, 10, APP_KeyPoll, NULL);
}

static void APP_BlueLedOff(void *p) {
	LED_BLUE_Off();
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
	uint8_t i;
	BLOCK_Object block;
	
    switch(event) {
        case EVNT_INIT: 
			LED_BLUE_On();
			WAIT_Waitms(500);
			LED_BLUE_Off();
			TRG_SetTrigger(TRG_LED_BLINK, 500, APP_Blink, NULL);
			TRG_SetTrigger(TRG_KEY_POLL, 10, APP_KeyPoll, NULL);
            break;
            
        case EVNT_HEARTBEAT:
        	LED_BLUE_Neg();
        	break;
        	
        case EVNT_SAVE_NVM:
        	LED_BLUE_On();
        	DB_SaveNVM();
        	TRG_SetTrigger(TRG_BLUE_LED_OFF, 1000, APP_BlueLedOff, NULL);
        	break;

        case EVNT_SERIAL_CMD:
        	switch(*SER_GetCommand()) {	
                /** Main Commands **/
                case SER_MODE:
                    ROB_SetRunMode(SER_GetData8(0));
                    SER_SendPacket(SER_MODE);
                    break;

                case SER_RUN:
                    ROB_Start();
                    SER_SendPacket(SER_RUN);
                    break;

                case SER_MOVETO_POSITION:
                    ROB_MoveToXYZ(SER_GetData16(0), SER_GetData16(2), SER_GetData16(4));
                    SER_SendPacket(SER_MOVETO_POSITION);
                    break;

                case SER_PUSH_BLOCK_SINGLE:
                    block.x = SER_GetData16(0);
                    block.y = SER_GetData16(2);
                    BLOCK_Push(block);
                    SER_SendPacket(SER_PUSH_BLOCK_SINGLE);
                    break;

                case SER_PUSH_BLOCK_ARRAY:
                    for(i=0; i<((*SER_GetLength())-5)/4; i++) {
                        block.x = SER_GetData16(4*i);
                        block.y = SER_GetData16(4*i+2);
                        BLOCK_Push(block);
                    }
                    SER_SendPacket(SER_PUSH_BLOCK_ARRAY);           
                    break;

                case SER_GET_POSITION:
                    SER_AddData16(rotary.position);
                    SER_AddData16(knee.position);
                    SER_AddData16(lift.position);
                    SER_SendPacket(SER_GET_POSITION);
                    break;

				/** Configuration Commands **/
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
						case POS: {
							BLOCK_Object* obj = (BLOCK_Object*) DB_GetVar(SER_GetData8(0));
							SER_AddData16(obj->x);
							SER_AddData16(obj->y);
							SER_AddData16(obj->h);
							break;
						}
						case T_DBGBUFFER: {
							uint8_t i;
							for(i=0; i<=SER_DEBUGBUFFER_LENGTH; i++) {
								SER_AddData8(debugBuffer[i]);
							}
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
                            MOT_RecalcValues(&rotary);
                            MOT_RecalcValues(&knee);
                            MOT_RecalcValues(&lift);
							break;
						}
						case POS: {
							((BLOCK_Object*) DB_GetVar(SER_GetData8(0)))->x = SER_GetData16(1);
							((BLOCK_Object*) DB_GetVar(SER_GetData8(0)))->y = SER_GetData16(3);
							((BLOCK_Object*) DB_GetVar(SER_GetData8(0)))->h = SER_GetData16(5);
							break;
						}
						case T_DBGBUFFER: {
							// we're using this just do delete variable content...
							SER_ResetDebugBuffer();
							DB_SaveNVM();
							break;
						}
					}
	
					SER_SendPacket(SER_WRITE_VARIABLE);            	
					break;
					
				case SER_SAVE_NVM:
					DB_SaveNVM();
					SER_SendPacket(SER_SAVE_NVM);
					break;     	
				
				case SER_DEBUG_PACKET: 
					SER_AddData16((uint16_t) MOT_GetState(&rotary));
					SER_AddData16((uint16_t) MOT_GetState(&knee));
					SER_AddData16((uint16_t) MOT_GetState(&lift));
					SER_AddData16(rotary.position);
					SER_AddData16(knee.position);
					SER_AddData16(lift.position);
					SER_AddData16((uint16_t) BLOCK_GetSize());
                    SER_AddData16((uint16_t) ROB_GetRunMode());
                    SER_AddData16((uint16_t) BLOCK_GetState());
					SER_SendPacket(SER_DEBUG_PACKET);
					break;
        	
				/*********** OLD COMMANDS DOWN HERE ***********/
                case '1':
                    LED_RED_On();
                    SER_SendPacket('1');
                    break;

                case '2':
                    LED_RED_Off();
                    SER_SendPacket('2');
                    break;
                    
                default:
                	// send error message
                	SER_SendPacket('E');
                    break;
        	}

        	SER_SetHandled();
        	break;
        	
        default:
            break;
    }
}
