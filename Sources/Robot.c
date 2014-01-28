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
	return (uint16_t) MOT_GetState(&rotary);
}

void ROB_MoveTo(uint16_t x, uint16_t y) {
	MOT_MoveSteps(&rotary, (int16_t) (x-rotary.position));
	MOT_MoveSteps(&knee,   (int16_t) (y-knee.position));
}

void ROB_Run(void) {
	BLOCK_Object block;
	
	switch(runmode) {
		case ROB_INIT:
			/* initialise the robot, startup and system test, go to zero pos */
			rotary.position = 0;
			knee.position   = 0;
			lift.position   = 500;
			break;

		case ROB_COLLECT:
			/* collect all blocks from block stack */
			if(!(ROB_Moving())) {
				if(BLOCK_GetSize() > 0) {			// if there are blocks in blockstack
					block = BLOCK_Pop();			// pop next block and set new target position
					ROB_MoveTo(block.x, block.y);
					MOT_MoveSteps(&lift,   (int16_t) (150-lift.position));
					runmode = ROB_COLLECT_PICK;
				}
				else if(BLOCK_GetSize() == 0) {
					runmode = ROB_IDLE;
				}
			}
			break;
			
		case ROB_COLLECT_PICK:
			/* wait until arm is at block location, then pick */
			if(!(ROB_Moving())) {					// wait for the last move to be finished
				// set Z target
				MOT_MoveSteps(&lift,   (int16_t) (100-lift.position));
				WAIT_Waitms(100);
				runmode = ROB_COLLECT_PICKED;
			}	
			break;
			
		case ROB_COLLECT_PICKED: 
			/* wait for the lift to be lowered, switch vaccuum on and move to center */
			if(!(ROB_Moving())) {					// wait for the last move to be finished
				// vacuum on
				LED_RED_On();
				
				// Move to center
				ROB_MoveTo(750, 750);
				MOT_MoveSteps(&lift,   (int16_t) (500-lift.position));
				runmode = ROB_COLLECT_RELEASE;
			}
			break;
			
		case ROB_COLLECT_RELEASE:
			/* wait until arm is at center location, then set z location */
			if(!(ROB_Moving())) {					// wait for the last move to be finished
				// set Z target
				MOT_MoveSteps(&lift,   (int16_t) (150-lift.position));
				
				WAIT_Waitms(100);
				runmode = ROB_COLLECT_RELEASED;
			}
			break;
			
		case ROB_COLLECT_RELEASED:
			/* wait for the lift to be lowered, switch vaccum off and move arm up */
			if(!(ROB_Moving())) {					// wait for the last move to be finished
				// vacuum off
				LED_RED_Off();
				
				// set Z target
				MOT_MoveSteps(&lift,   (int16_t) (500-lift.position));
				runmode = ROB_COLLECT;
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
