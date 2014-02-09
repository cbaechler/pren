/**
 * \file
 * \brief Robot Module main file.
 * \author Christoph Bächler
 * \date 04.01.2014
 *
 * This file contains the main code for the robot. It 
 * implements the different run modes (init, collect, debug, ...)
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

void ROB_Run(void) {
	switch(runmode) {
		case ROB_INIT:
			/* initialise the robot, startup and system test, go to zero pos */
			rotary.position = 0;
			knee.position   = 0;
			lift.position   = 25000;
			
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
