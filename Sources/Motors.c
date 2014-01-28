/**
 * \file
 * \brief Motor module implementation.
 * \author Christoph Bächler
 * \date 16.12.2013
 */

#include "PE_Types.h"
#include "Math.h"
#include "Motors.h"
#include "ILIM.h"

#include "M1_MODE0.h"
#include "M1_MODE1.h"
#include "M1_MODE2.h"
#include "M1_FAULT.h"
#include "M1_NRST.h"
#include "M1_DIR.h"

#include "M2_MODE0.h"
#include "M2_MODE1.h"
#include "M2_MODE2.h"
#include "M2_FAULT.h"
#include "M2_NRST.h"
#include "M2_DIR.h"

#include "M3_MODE0.h"
#include "M3_MODE1.h"
#include "M3_MODE2.h"
#include "M3_FAULT.h"
#include "M3_NRST.h"
#include "M3_DIR.h"

#include "LED_RED.h"

uint16_t OCR1A;	/* emulate atmel register */

MOT_FSMData rotary;	/* Drehachse */
MOT_FSMData knee;	/* Knickgelenk */
MOT_FSMData lift;	/* Hebemechanismus */

static LDD_TDeviceData *ILIM_Ptr;

/* This will initialise the Motor module */
void MOT_Init(void) {
	// M1
	rotary.index = ROTARY;
	rotary.running = FALSE;
	rotary.state = MOT_FSM_STOP;
	MOT_SetStepMode(&rotary, MOT_STEP_1);
	
	// M2
	knee.index = KNEE;
	knee.running = FALSE;
	knee.state = MOT_FSM_STOP;
	MOT_SetStepMode(&knee, MOT_STEP_1);

	// M3
	lift.index = LIFT;
	lift.running = FALSE;
	lift.state = MOT_FSM_STOP;
	MOT_SetStepMode(&lift, MOT_STEP_1);
	
	// I Limit
	ILIM_Ptr = ILIM_Init(NULL);
	MOT_SetILim(1000);
}

void MOT_SetILim(uint16_t i_max) {
	// V_ref = 
	// i_max = 2 * v_out
	// --> v_out = i_max/2
	// 1 A -> 1000/2 -> 500 mV
	ILIM_SetValue(ILIM_Ptr, (uint16_t) (i_max/2)*(4096/7200));
}

void MOT_SetStepMode(MOT_FSMData* m_, uint8_t step_mode) {
	// set new step mode
	LED_RED_On();	// use this to determine the duration of following code
	switch(m_->index) {
		case ROTARY: 
			M1_MODE0_PutVal((step_mode & 1)>>0);
			M1_MODE1_PutVal((step_mode & 2)>>1);
			M1_MODE2_PutVal((step_mode & 4)>>2);
			// TODO: recalc all values!
			break;
			
		case KNEE: 
			M2_MODE0_PutVal((step_mode & 1)>>0);
			M2_MODE1_PutVal((step_mode & 2)>>1);
			M2_MODE2_PutVal((step_mode & 4)>>2);
			// TODO: recalc all values!
			break;
			
		case LIFT: 
			M3_MODE0_PutVal((step_mode & 1)>>0);
			M3_MODE1_PutVal((step_mode & 2)>>1);
			M3_MODE2_PutVal((step_mode & 4)>>2);
			// TODO: recalc all values!
			break;
	}	
	LED_RED_Off();
}

bool MOT_GetFaultState(MOT_FSMData* m_) {
	switch(m_->index) {
		case ROTARY:	return M1_FAULT_GetVal();		break;
		case KNEE:		return M2_FAULT_GetVal();		break;
		case LIFT:		return M3_FAULT_GetVal();		break;
	}
	return TRUE;
}

uint8_t MOT_GetState(MOT_FSMData* m_) {
	uint8_t result;
	result = 0;
	
	switch(m_->index) {
		case ROTARY:
			result |= (M1_FAULT_GetVal()<<0);
			result |= (M1_MODE0_GetVal()<<1);
			result |= (M1_MODE1_GetVal()<<2);
			result |= (M1_MODE2_GetVal()<<3);
			result |= (M1_DIR_GetVal()<<4);
			result |= (M1_nRST_GetVal()<<5);
			break;
			
		case KNEE:
			result |= (M2_FAULT_GetVal()<<0);
			result |= (M2_MODE0_GetVal()<<1);
			result |= (M2_MODE1_GetVal()<<2);
			result |= (M2_MODE2_GetVal()<<3);
			result |= (M2_DIR_GetVal()<<4);
			result |= (M2_nRST_GetVal()<<5);
			break;
			
		case LIFT:
			result |= (M3_FAULT_GetVal()<<0);
			result |= (M3_MODE0_GetVal()<<1);
			result |= (M3_MODE1_GetVal()<<2);
			result |= (M3_MODE2_GetVal()<<3);
			result |= (M3_DIR_GetVal()<<4);
			result |= (M3_nRST_GetVal()<<5);
			break;
	}
	return result;
}

/*! This will calculate the values for the motor to speed
 * \param m_	 Pointer to the motor object
 * \param accel  Accelration to use, in 0.01*rad/sec^2.
 * \param decel  Decelration to use, in 0.01*rad/sec^2.
 * \param speed  Max speed, in 0.01*rad/sec.
 */
void MOT_CalcValues(MOT_FSMData* m_, uint16_t accel, uint16_t decel, uint16_t speed) {
	m_->accel = accel;
	m_->decel = decel; 
	m_->speed = speed;
	m_->position = 0;
	
	// Set max speed limit, by calc min_delay to use in timer.
	// min_delay = (alpha / tt)/ w
	m_->min_delay = A_T_x100 / speed;
		
	// Find out after how many steps does the speed hit the max speed limit.
	// max_s_lim = speed^2 / (2*alpha*accel)
	m_->max_s_lim = (int32_t)speed*speed/(int32_t)(((int32_t)A_x20000*accel)/100);
	// If we hit max speed limit before 0,5 step it will round to 0.
	// But in practice we need to move atleast 1 step to get any speed at all.
	if(m_->max_s_lim == 0) {
		m_->max_s_lim = 1;
	}
}

/*! This tells the motor to drive any number of steps 
 * \param m_	 Pointer to the motor object
 * \param steps  Number of steps to move (pos - CW, neg - CCW).
 */
void MOT_MoveSteps(MOT_FSMData* m_, int16_t steps) {
	// Set direction from sign on step value.
	if(steps < 0) {
		m_->dir = CCW;
		steps = -steps;
	}
	else {
		m_->dir = CW;
	}
	
	// If moving only 1 step.
	if(steps == 1) {
		m_->accel_count = -1;
		m_->state = MOT_FSM_DECEL;
		m_->step_delay = 1000;
		m_->running = TRUE;
		OCR1A = 10;
	}
	else if(steps != 0) {		
		// Find out after how many steps we must start deceleration.
		// n1 = (n1+n2)decel / (accel + decel)
		m_->accel_lim = ((long)steps*m_->decel) / (m_->accel+m_->decel);
		// We must accelrate at least 1 step before we can start deceleration.
		if(m_->accel_lim == 0) {
			m_->accel_lim = 1;
		}
		
		// Use the limit we hit first to calc decel.
		if(m_->accel_lim <= m_->max_s_lim) {
			m_->decel_val = m_->accel_lim - steps;
		}
		else {
			m_->decel_val = -((int32_t)m_->max_s_lim*m_->accel)/m_->decel;
		}
		
		// We must decelrate at least 1 step to stop.
		if(m_->decel_val == 0) {
			m_->decel_val = -1;
		}
		
		// Find step to start decleration.
		m_->decel_start = steps + m_->decel_val;
		
		// Set accelration by calc the first (c0) step delay .
		// step_delay = 1/tt * sqrt(2*alpha/accel)
		// step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
		m_->step_delay = (T1_FREQ_148 * MATH_sqrt(A_SQ / m_->accel))/100;
		
		// If the maximum speed is so low that we dont need to go via accelration state.
		if(m_->step_delay <= m_->min_delay) {
			m_->step_delay = m_->min_delay;
			m_->state = MOT_FSM_RUN;
		}
		else {
			m_->state = MOT_FSM_ACCEL;
		}
		
		// Reset counter.
		m_->step_count = 0;
		m_->rest = 0;
		m_->accel_count = 0;
		m_->running = TRUE;

		OCR1A = 10;
	}
}


uint16_t MOT_Process(MOT_FSMData* m_) {
	uint16_t new_step_delay;

	OCR1A = m_->step_delay;
	
	if(m_->running) {
		if(m_->dir == CW) {
			m_->position++;
		}
		else {
			m_->position--;
		}
	}
	
	switch(m_->state) {	
		case MOT_FSM_STOP:
			m_->step_count = 0;
			m_->rest = 0;
			m_->running = FALSE;
			break;
		
		case MOT_FSM_ACCEL:
			//sm_driver_StepCounter(srd.dir);
			m_->step_count++;
			m_->accel_count++;
			new_step_delay = m_->step_delay - (((2 * (int32_t)m_->step_delay) + m_->rest)/(4 * m_->accel_count + 1));
			m_->rest = ((2 * (int32_t)m_->step_delay)+m_->rest)%(4 * m_->accel_count + 1);
			// Chech if we should start deceleration.
			if(m_->step_count >= m_->decel_start) {
				m_->accel_count = m_->decel_val;
				m_->state = MOT_FSM_DECEL;
			}
			// Check if we hitted max speed.
			else if(new_step_delay <= m_->min_delay) {
				m_->last_accel_delay = new_step_delay;
				new_step_delay = m_->min_delay;
				m_->rest = 0;
				m_->state = MOT_FSM_RUN;
			}
			break;
		
		case MOT_FSM_RUN:
			//sm_driver_StepCounter(srd.dir);
			m_->step_count++;
			new_step_delay = m_->min_delay;
			// Check if we should start deceleration.
			if(m_->step_count >= m_->decel_start) {
				m_->accel_count = m_->decel_val;
				// Start deceleration with same delay as accel ended with.
				new_step_delay = m_->last_accel_delay;
				m_->state = MOT_FSM_DECEL;
			}
			break;
			
		case MOT_FSM_DECEL:
			//sm_driver_StepCounter(srd.dir);
			m_->step_count++;
			m_->accel_count++;
			new_step_delay = m_->step_delay - (((2 * (int32_t)m_->step_delay) + m_->rest)/(4 * m_->accel_count + 1));
			m_->rest = ((2 * (int32_t)m_->step_delay)+m_->rest)%(4 * m_->accel_count + 1);
			// Check if we at last step
			if(m_->accel_count >= 0) {
				m_->state = MOT_FSM_STOP;
			}
			break;
	}
	m_->step_delay = new_step_delay;
	
	return OCR1A;
}


