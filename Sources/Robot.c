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

static ROB_RunMode runmode;

void ROB_Init(void) {
	runmode = ROB_INIT;
}

void ROB_SetRunMode(ROB_RunMode mode) {
	runmode = mode;
}

void ROB_Run(void) {
	switch(runmode) {
		case ROB_INIT:
			/* */
			break;

		case ROB_COLLECT:
			/* for num of blocks in block store */


			break;

		case ROB_DEBUG:
			break;

		case ROB_SCAN:
			break;

		default:
			break;
	}
}
