
#ifndef MOTORS_H_
#define MOTORS_H_

typedef enum MOT_Dir {
	CCW, CW
} MOT_Dir;

typedef enum MOT_StateKinds {
	MOT_FSM_IDLE, 
	MOT_FSM_STOP, 
	MOT_FSM_ACCEL, 
	MOT_FSM_RUN, 
	MOT_FSM_DECEL
} MOT_StateKinds;

typedef struct MOT_FSMData {
	MOT_StateKinds state;		// ok
	MOT_Dir dir;				// ok
	bool running;				// ok (ev. no used)

	/* max speed, max accel, max deccel */
	uint16_t accel;				// ok
	uint16_t decel;				// ok
	uint16_t speed;				// ok

	/* calced values ... */
	int16_t min_delay;			// ok
	uint16_t step_delay;		// ok
	uint16_t max_s_lim;			// ok

	/* variables */
	uint16_t accel_lim;			// ok
	int16_t accel_count;		// ok
	int16_t decel_val;			// ok
	uint16_t decel_start;		// ok
	int16_t last_accel_delay;
	uint16_t rest;

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
#define T1_FREQ 460750

//! Number of (full)steps per round on stepper motor in use.
#define FSPR 200
#define SPR FSPR

// Maths constants. To simplify maths when calculating in speed_cntr_Move().
#define ALPHA (2*3.14159/SPR)                    // 2*pi/spr
#define A_T_x100 ((int32_t)(ALPHA*T1_FREQ*100))     // (ALPHA / T1_FREQ)*100
#define T1_FREQ_148 ((int16_t)((T1_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676
#define A_SQ (int32_t)(ALPHA*2*10000000000)         // ALPHA*2*10000000000
#define A_x20000 (int16_t)(ALPHA*20000)              // ALPHA*20000

void MOT_Init(void);

void MOT_CalcValues(MOT_FSMData* m_, uint16_t accel, uint16_t decel, uint16_t speed);

void MOT_MoveSteps(MOT_FSMData* m_, int16_t steps);

void MOT_Process(MOT_FSMData* m_);






#endif /* MOTORS_H_ */
