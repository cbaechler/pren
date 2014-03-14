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
#include "WAIT.h"

static BLOCK_Object block_storage[BLOCK_STACK_MAX_SIZE];
static BLOCK_Object nullblock = {0,0};
static uint8_t block_index;
static BLOCK_FSMData data = {BLOCK_IDLE, FALSE};

void BLOCK_Init(void) {
	BLOCK_Clear();
}

void BLOCK_StartPickPlace(void) {
	data.started = TRUE;
	data.state = BLOCK_NEXT;
}

uint8_t BLOCK_PickPlace_GetState(void) {
	return (uint8_t) data.state;
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
					ROB_MoveTo(block.x, block.y);
					MOT_MoveSteps(&lift,   (int16_t) (5000-lift.position));
					data.state = BLOCK_PICK;
				}
				else if(BLOCK_GetSize() == 0) {
					data.started = FALSE;
					data.state = BLOCK_IDLE;
				}
			}
			break;
			
		case BLOCK_PICK:
			/* wait until arm is at block location, then pick */
			if(!(ROB_Moving())) {					// wait for the last move to be finished
				// set Z target
				MOT_MoveSteps(&lift,   (int16_t) (1000-lift.position));
				WAIT_Waitms(100);
				data.state = BLOCK_PICKED;
			}	
			break;
			
		case BLOCK_PICKED: 
			/* wait for the lift to be lowered, switch vaccuum on and move to center */
			if(!(ROB_Moving())) {					// wait for the last move to be finished
				// vacuum on
				HW_LED(RED, TRUE);
				HW_VALVE(TRUE);

				// Move to center
				ROB_MoveTo(750, 750);
				MOT_MoveSteps(&lift,   (int16_t) (25000-lift.position));
				data.state = BLOCK_RELEASE;
			}
			break;
			
		case BLOCK_RELEASE:
			/* wait until arm is at center location, then set z location */
			if(!(ROB_Moving())) {					// wait for the last move to be finished
				// set Z target
				MOT_MoveSteps(&lift,   (int16_t) (5000-lift.position));
				
				WAIT_Waitms(100);
				data.state = BLOCK_RELEASED;
			}
			break;
			
		case BLOCK_RELEASED:
			/* wait for the lift to be lowered, switch vaccum off and move arm up */
			if(!(ROB_Moving())) {					// wait for the last move to be finished
				// vacuum off
				HW_LED(RED, FALSE);
				HW_VALVE(FALSE);
				
				// set Z target
				MOT_MoveSteps(&lift,   (int16_t) (25000-lift.position));
				data.state = BLOCK_NEXT;
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
