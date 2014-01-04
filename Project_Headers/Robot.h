/**
 * \file
 * \brief Robot module header file.
 * \author Christoph Bächler
 * \date 04.01.2014
 */

#ifndef ROBOT_H_
#define ROBOT_H_

#define ROB_INIT 		'I'
#define ROB_COLLECT 	'C'
#define ROB_DEBUG 		'D'
#define ROB_SCAN 		'S'

typedef uint8_t ROB_RunMode;

typedef struct ROB_Position {

} ROB_Position;

void ROB_Init(void);
void ROB_SetRunMode(ROB_RunMode mode);
void ROB_Run(void);


#endif /* ROBOT_H_ */
