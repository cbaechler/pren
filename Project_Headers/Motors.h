/**
 * \file
 * \brief Motor module header file.
 * \author Christoph Bächler
 * \date 16.12.2013
 */

#ifndef MOTORS_H_
#define MOTORS_H_

// Define Step Modes
#define MOT_STEP_1		0	//!< Full step mode
#define MOT_STEP_2		1	//!< Half step mode
#define MOT_STEP_4		2	//!< Quarter step mode
#define MOT_STEP_8		3	//!< 1/8 step mode
#define MOT_STEP_16		4	//!< 1/16 step mode
#define MOT_STEP_32		5	//!< 1/32 step mode

// Define Motor Index
#define	ROTARY	1
#define KNEE	2
#define LIFT	3

// Define Motor Directions
#define CCW		0
#define CW		1

typedef enum MOT_Pins {
	MOT_PIN_MODE0, 
	MOT_PIN_MODE1, 
	MOT_PIN_MODE2, 
	MOT_PIN_NRST, 
	MOT_PIN_DIR, 
	MOT_PIN_FAULT, 
	MOT_PIN_LIM
} MOT_Pins;

// Define Motor FSM States
typedef enum MOT_StateKinds {
	MOT_FSM_STOP, 
	MOT_FSM_ACCEL, 
	MOT_FSM_RUN, 
	MOT_FSM_DECEL
} MOT_StateKinds;

// Define Public Motor Data Cluster (used for communication)
typedef struct MOT_PubData {
	uint16_t accel;
	uint16_t decel;
	uint16_t speed;
} MOT_PubData;

// Define Internal Motor Data Cluster
typedef struct MOT_FSMData {
	uint8_t index;				// ok
	MOT_StateKinds state;		// ok
	bool dir;					// ok
	bool running;				// ok
	
	/* motor settings */
	MOT_PubData p;
	/*
	uint16_t accel;				//!<eeprom
	uint16_t decel;				//!<eeprom
	uint16_t speed;				//!<eeprom
	*/
	/* pre-calculated values ... */
	int16_t min_delay;			// ok
	uint16_t max_s_lim;			// ok

	/* variables */
	uint16_t step_delay;		// ok
	uint16_t accel_lim;			// ok
	int16_t accel_count;		// ok
	int16_t decel_val;			// ok
	uint16_t decel_start;		// ok
	int16_t last_accel_delay;
	uint16_t rest;
	uint16_t position;

	/* setpoints */
	uint16_t step_count;		// ok
} MOT_FSMData;

extern MOT_FSMData rotary;	/* Drehachse */
extern MOT_FSMData knee;	/* Knickgelenk */
extern MOT_FSMData lift;	/* Hebemechanismus */

/*! \Brief Frequency of timer1 in [Hz].
 *
 * Modify this according to frequency used. Because of the prescaler setting,
 * the timer1 frequency is the clock frequency divided by 8.
 */
// Timer/Counter 1 running on 3,686MHz / 8 = 460,75kHz (2,17uS). (T1-FREQ 460750)
#define T1_FREQ 187500	/* TODO: check if that is correct */

//! Number of (full)steps per round on stepper motor in use.
#define FSPR 200
#define SPR  FSPR
//#define SPR (FSPR*32)	/* TODO: when chaning step mode... also change SPR, recalc! */

// Maths constants
#define ALPHA (2*3.14159/SPR)                    		// 2*pi/spr
#define A_T_x100 ((int32_t)(ALPHA*T1_FREQ*100))     	// (ALPHA / T1_FREQ)*100
#define T1_FREQ_148 ((int16_t)((T1_FREQ*0.676)/100)) 	// divided by 100 and scaled by 0.676
#define A_SQ (int32_t)(ALPHA*2*10000000000)         	// ALPHA*2*10000000000
#define A_x20000 (int16_t)(ALPHA*20000)              	// ALPHA*20000

void MOT_Init(void);
void MOT_SetILim(uint16_t i_max);
void MOT_SetStepMode(MOT_FSMData* m_, uint8_t step_mode);
void MOT_SetDirection(MOT_FSMData* m_, bool dir);
bool MOT_GetFaultState(MOT_FSMData* m_);
uint8_t MOT_GetState(MOT_FSMData* m_);
void MOT_CalcValues(MOT_FSMData* m_, uint16_t accel, uint16_t decel, uint16_t speed);
void MOT_MoveSteps(MOT_FSMData* m_, int16_t steps);
uint16_t MOT_Process(MOT_FSMData* m_);

#endif /* MOTORS_H_ */
