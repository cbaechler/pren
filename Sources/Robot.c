/**
 * \file
 * \brief Robot Module main file.
 * \author Christoph Bächler
 * \date 04.01.2014
 *
 * This file contains the main code for the robot. It 
 * implements the different run modes (init, collect, debug, ...) 
 * and connects the hardware. 
 */

#include "PE_Types.h"
#include "Robot.h"
#include "BlockStack.h"
#include "Motors.h"
#include "WAIT.h"
#include "VALVE.h"
#include "LED_RED.h"
#include "LED_BLUE.h"
#include "LED_GREEN.h"
#include "LED_S1.h"
#include "LED_S2.h"
#include "LED_ER.h"

static ROB_RunMode runmode;
static bool running;

void ROB_Init(void) {
	runmode = ROB_IDLE;
}

void ROB_SetRunMode(ROB_RunMode mode) {
	runmode = mode;
}

uint8_t ROB_GetRunMode(void) {
	return (uint8_t) runmode;
}

uint16_t ROB_GetStateArray(void) {
	/*
	uint16_t array;
	array = 0;
	array |= (M1_MODE0_GetVal()<<0);
	array |= (M1_MODE1_GetVal()<<1);
	array |= (M1_MODE2_GetVal()<<2);
	return array;*/
	return (uint16_t) MOT_GetState(&lift);
}

void ROB_MoveToXY(uint16_t x, uint16_t y) {
	MOT_MoveSteps(&rotary, (int16_t) (x-rotary.position));
	MOT_MoveSteps(&knee,   (int16_t) (y-knee.position));
}

void ROB_MoveToXYZ(uint16_t x, uint16_t y, uint16_t z) {
	MOT_MoveSteps(&rotary, (int16_t) (x-rotary.position));
	MOT_MoveSteps(&knee,   (int16_t) (y-knee.position));
	MOT_MoveSteps(&lift,   (int16_t) (z-lift.position));
}

void ROB_MoveToZ(uint16_t z) {
	MOT_MoveSteps(&lift,   (int16_t) (z-lift.position));
}

void HW_VALVE(bool state) {
	VALVE_PutVal(state);
}

void HW_LED(uint8_t led, bool state) {
	switch(led) {
		case RED:
			LED_RED_Put(state);
			break;

		case BLUE:
			LED_BLUE_Put(state);
			break;

		case GREEN:
			LED_GREEN_Put(state);
			break;

		case S1:
			LED_S1_Put(state);
			break;

		case S2:
			LED_S2_Put(state);
			break;

		case ER:
			LED_ER_Put(state);
			break;
	}
}

void ROB_Start(void) {
	running = TRUE;
}

void ROB_Process(void) {
	if(running) {
		switch(runmode) {
			case ROB_INIT:
				/* initialize the robot, startup and system test, go to zero pos */
				MOT_MoveToLim(&rotary, 4, CW);
				MOT_MoveToLim(&knee, 2, CW);
				MOT_MoveToLim(&lift, 2, CCW);
				
				//MOT_MoveToLim(&lift, 1, CCW);	// add the other motors
				
				/* set the positions that describe limit position */
				rotary.position = lim_position.x;
				knee.position = lim_position.y;
				lift.position = lim_position.h;		
				
				/* go to HOME position */
				//BLOCK_MoveToBlockPos(home_position);
				//ROB_MoveToZ(0);
				
				/* wait until HOME reached */
				//while(ROB_Moving());
				
				/* now we are ready to win */
				runmode = ROB_IDLE;
				break;

			case ROB_COLLECT:
				break;

			case ROB_PICKPLACE:
				BLOCK_StartPickPlace();
				runmode = ROB_PICKPLACE_PROCESS;
				break;

			case ROB_PICKPLACE_PROCESS:
				BLOCK_PickPlace_Process();
				if(BLOCK_PickPlace_GetState() == BLOCK_IDLE) {
					runmode = ROB_IDLE;
				}
				break;

			case ROB_DEBUG:
				break;

			case ROB_SCAN:
				break;
				
			case ROB_IDLE:
				running = FALSE;
				break;

			default:
				break;
		}
	}
}

bool ROB_Moving(void) {
	return (rotary.running | knee.running | lift.running);
}
