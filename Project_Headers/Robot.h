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

typedef uint8_t ROB_RunMode;

typedef struct ROB_Position {

} ROB_Position;

void ROB_Init(void);
void ROB_SetRunMode(ROB_RunMode mode);
uint8_t ROB_GetRunMode(void);
uint16_t ROB_GetStateArray(void);
void ROB_Run(void);
bool ROB_Moving(void);
void ROB_MoveTo(uint16_t x, uint16_t y);

#endif /* ROBOT_H_ */
