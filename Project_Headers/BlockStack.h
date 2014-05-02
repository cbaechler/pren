/**
 * \file
 * \brief Block Store module header file.
 * \author Christoph Bächler
 * \date 04.01.2014
 */

#include "PE_Types.h"

typedef struct BLOCK_Object {
	uint16_t x;
	uint16_t y;
	uint16_t h;
} BLOCK_Object;

typedef enum BLOCK_StateKinds {
	BLOCK_IDLE,
	BLOCK_NEXT,
	BLOCK_PICK,
	BLOCK_PICKED,
	BLOCK_CENTER,
	BLOCK_RELEASE,
	BLOCK_RELEASED
} BLOCK_StateKinds;

typedef struct BLOCK_FSMData {
	BLOCK_StateKinds state;
	bool started;
	uint8_t nof_processed_blocks;
} BLOCK_FSMData;

#define BLOCK_STACK_MAX_SIZE	10

extern BLOCK_Object lim_position; 	/* Position that robot reaches in initialisation (lim switch) */
extern BLOCK_Object home_position;	/* Position where robot is after init and on end */
extern BLOCK_Object stack_position;	/* Position where the stack will be built */
extern uint16_t zBlockHeight;		/* Height of one single block */
extern uint16_t zTargetSurface;		/* Distance to Target Surface (center) */
extern uint16_t zGroundSurface;		/* Distance to Ground Surface (maximum) */

void BLOCK_Init(void);
void BLOCK_StartPickPlace(void);
uint8_t BLOCK_PickPlace_GetState(void);
void BLOCK_MoveToBlockPos(BLOCK_Object xypos);
void BLOCK_PickPlace_Process(void);
bool BLOCK_IsEmtpy(void);
bool BLOCK_IsFull(void);
void BLOCK_Push(BLOCK_Object obj);
BLOCK_Object BLOCK_Pop(void);
BLOCK_Object BLOCK_GetSingle(uint8_t index);
uint8_t BLOCK_GetSize(void);
uint8_t BLOCK_GetState(void);
void BLOCK_Clear(void);
