/**
 * \file
 * \brief Block Store module implementation.
 * \author Christoph Bächler
 *
 * BlockStorage is the database to store block positions. 
 * It is implemented as a simple stack containing 
 * robot arm positions for every block. The data 
 * structure allows a maximum of 10 blocks to be stored.
 */

#include "PE_Types.h"
#include "Cpu.h"
#include "Robot.h"
#include "Motors.h"
#include "BlockStack.h"
#include "Robot.h"
#include "WAIT.h"

static BLOCK_Object block_storage[BLOCK_STACK_MAX_SIZE];
static BLOCK_Object nullblock = {0,0};
static uint8_t block_index;
static BLOCK_FSMData data = {BLOCK_IDLE, FALSE, 0};

BLOCK_Object lim_position;
BLOCK_Object home_position;
BLOCK_Object stack_position;
uint16_t zBlockHeight;		/* Height of one single block */
uint16_t zTargetSurface;	/* Distance to Target Surface (stack position) */
uint16_t zGroundSurface;	/* Distance to Ground Surface (maximum) */

void BLOCK_Init(void) {
	BLOCK_Clear();
}

uint8_t BLOCK_GetState(void) {
	return data.state;
}

void BLOCK_StartPickPlace(void) {
	data.nof_processed_blocks = 0;
	data.started = TRUE;
	data.state = BLOCK_NEXT;
}

uint8_t BLOCK_PickPlace_GetState(void) {
	return (uint8_t) data.state;
}

void BLOCK_MoveToBlockPos(BLOCK_Object xypos) {
	ROB_MoveToXY(xypos.x, xypos.y);
}

/*! \brief Pick and Place routine. 
 *
 * This function handles the FSM for pick and place logic. It will automatically collect 
 * all blocks that have been pushed to the block stack. 
 */
void BLOCK_PickPlace_Process(void) {
	BLOCK_Object block;
	
	switch(data.state) {
		case BLOCK_IDLE:
			break;

		case BLOCK_NEXT:
			/* collect all blocks from block stack */
			if(!(ROB_Moving())) {
				if(BLOCK_GetSize() > 0) {			// if there are blocks in blockstack
					block = BLOCK_Pop();			// pop next block and set new target position
					BLOCK_MoveToBlockPos(block);
					ROB_MoveToZ(zGroundSurface - 2*zBlockHeight);
					data.state = BLOCK_PICK;
				}
				else if(BLOCK_GetSize() == 0) {
					BLOCK_MoveToBlockPos(home_position);
					data.started = FALSE;
					data.state = BLOCK_IDLE;
				}
			}
			break;
			
		case BLOCK_PICK:
			/* wait until arm is at block location, then pick */
			if(!(ROB_Moving())) {					// wait for the last move to be finished
				// set Z target
				ROB_MoveToZ(zGroundSurface - zBlockHeight);				// go down to the block
				//WAIT_Waitms(500);
				data.state = BLOCK_PICKED;
			}	
			break;
			
		case BLOCK_PICKED: 
			/* wait for the lift to be lowered, switch vaccuum on and move to center */
			if(!(ROB_Moving())) {					// wait for the last move to be finished
				// vacuum on
				HW_VALVE(TRUE);

				// Move up
				ROB_MoveToZ(zTargetSurface - (data.nof_processed_blocks+2) * zBlockHeight);
				data.state = BLOCK_CENTER;
			}
			break;
			
		case BLOCK_CENTER:
		 	/* wait for the lift to have min height of block stack size, then go to stack location */
			if(lift.position > data.nof_processed_blocks * zBlockHeight) {
				BLOCK_MoveToBlockPos(stack_position);
				data.state = BLOCK_RELEASE;
			}
			break;
			
		case BLOCK_RELEASE:
			/* wait until arm is at stack location, then set z location */
			if(!(ROB_Moving())) {					// wait for the last move to be finished
				// set Z target
				ROB_MoveToZ(zTargetSurface - (data.nof_processed_blocks+1) * zBlockHeight);
				//WAIT_Waitms(500);
				data.state = BLOCK_RELEASED;
			}
			break;
			
		case BLOCK_RELEASED:
			/* wait for the lift to be lowered, switch vaccum off and move arm up */
			if(!(ROB_Moving())) {					// wait for the last move to be finished
				// vacuum off
				HW_VALVE(FALSE);
				
				// set Z target
				//MOT_MoveSteps(&lift,   (int16_t) (25000-lift.position));
				ROB_MoveToZ(zTargetSurface - (data.nof_processed_blocks+2) * zBlockHeight);
				
				data.state = BLOCK_NEXT;
				data.nof_processed_blocks++;
			}
			break;

		default:
			data.state = BLOCK_IDLE;
			break;
	}
}

bool BLOCK_IsEmtpy(void) {
	return block_index < 0;
}

bool BLOCK_IsFull(void) {
	return block_index > BLOCK_STACK_MAX_SIZE-1;
}

void BLOCK_Push(BLOCK_Object obj) {
	if(!BLOCK_IsFull()) {
		EnterCritical();
		block_storage[++block_index] = obj;
		ExitCritical();
	}	
}

BLOCK_Object BLOCK_Pop(void) {
	BLOCK_Object obj = nullblock;
	if(!BLOCK_IsEmtpy()) {
		EnterCritical();
		obj = BLOCK_GetSingle(block_index--);
		ExitCritical();
	}
	return obj;
}

BLOCK_Object BLOCK_GetSingle(uint8_t index) {
	return block_storage[index];
}

uint8_t BLOCK_GetSize(void) {
	return block_index;
}

void BLOCK_Clear(void) {
	block_index = 0;
}
