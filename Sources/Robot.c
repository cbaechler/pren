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
#include "LED_RED.h"
#include "WAIT.h"
#include "M1_MODE0.h"
#include "M1_MODE1.h"
#include "M1_MODE2.h"
#include "VALVE.h"
#include "LED_RED.h"
#include "LED_BLUE.h"
#include "LED_GREEN.h"
#include "LED_S1.h"
#include "LED_S2.h"
#include "LED_ER.h"

static ROB_RunMode runmode;

void ROB_Init(void) {
	runmode = ROB_INIT;
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

void ROB_MoveTo(uint16_t x, uint16_t y) {
	MOT_MoveSteps(&rotary, (int16_t) (x-rotary.position));
	MOT_MoveSteps(&knee,   (int16_t) (y-knee.position));
}

/*bool ROB_CheckSystemValidity(void) {

}*/

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


void ROB_Run(void) {
	switch(runmode) {
		case ROB_INIT:
			/* initialise the robot, startup and system test, go to zero pos */
			rotary.position = 0;
			knee.position   = 0;
			lift.position   = 25000;

			/* move each axis until MOTx_LIM is reached */
			
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
			break;

		default:
			break;
	}
}

bool ROB_Moving(void) {
	return (rotary.running | knee.running | lift.running);
}
