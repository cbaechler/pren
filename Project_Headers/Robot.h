/**
 * \file
 * \brief Robot module header file.
 * \author Christoph Bächler
 * \date 04.01.2014
 */

#ifndef ROBOT_H_
#define ROBOT_H_

#define ROB_INIT 				'I'
#define ROB_COLLECT 			'X'
#define ROB_PICKPLACE			'C'
#define ROB_PICKPLACE_PROCESS	180
#define ROB_DEBUG 				'D'
#define ROB_SCAN 				'S'
#define ROB_IDLE				'P'

typedef enum {
	RED, 
	BLUE, 
	GREEN, 
	S1, 
	S2, 
	ER
} LED;

typedef uint8_t ROB_RunMode;

typedef struct ROB_Position {
	uint16_t alpha;
	uint16_t beta;
} ROB_Position;

void ROB_Init(void);
void ROB_SetRunMode(ROB_RunMode mode);
uint8_t ROB_GetRunMode(void);
uint16_t ROB_GetStateArray(void);
void ROB_Run(void);
bool ROB_Moving(void);
void ROB_MoveToZ(uint16_t z);
void ROB_MoveToXY(uint16_t x, uint16_t y);
void ROB_MoveToXYZ(uint16_t x, uint16_t y, uint16_t z);

/* These are direct hardware access commands */
void HW_VALVE(bool state);
void HW_LED(uint8_t led, bool state);

#endif /* ROBOT_H_ */
