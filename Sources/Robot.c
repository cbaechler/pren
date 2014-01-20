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

static ROB_RunMode runmode;

void ROB_Init(void) {
	runmode = ROB_INIT;
}

void ROB_SetRunMode(ROB_RunMode mode) {
	runmode = mode;
}

void ROB_Run(void) {
	BLOCK_Object block;
	
	switch(runmode) {
		case ROB_INIT:
			/* */
			break;

		case ROB_COLLECT:
			/* collect all blocks from block stack */
			// TODO: Every block on it's own, wait until header is there 
			//       switch on vacuum, go to center.
			if(!(ROB_Moving())) {
				if(BLOCK_GetSize() > 0) {
					block = BLOCK_Pop();
					MOT_MoveSteps(&rotary, (int16_t) (block.x-rotary.position));
					MOT_MoveSteps(&knee, (int16_t) (block.y-knee.position));
				}
				else if(BLOCK_GetSize() == 0) {
					runmode = ROB_IDLE;
				}
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
