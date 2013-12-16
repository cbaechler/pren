#include "PE_Types.h"
#include "AS1.h"
#include "Serial.h"
#include "Motors.h"

static uint32_t MATH_sqrt(uint32_t x);

bool running;
uint16_t OCR1A;	/* emulate atmel register */

MOT_FSMData m;

/* This will initialise the Motor module */
void MOT_Init(void) {
  m.state = MOT_FSM_STOP;
}

/*! This will calculate the values for the motor to speed
 *  \param accel  Accelration to use, in 0.01*rad/sec^2.
 *  \param decel  Decelration to use, in 0.01*rad/sec^2.
 *  \param speed  Max speed, in 0.01*rad/sec.
 */
void MOT_CalcValues(uint16_t accel, uint16_t decel, uint16_t speed) {
	m.accel = accel;
	m.decel = decel; 
	m.speed = speed;
	
	// Set max speed limit, by calc min_delay to use in timer.
	// min_delay = (alpha / tt)/ w
	m.min_delay = A_T_x100 / speed;
	
	// Set accelration by calc the first (c0) step delay .
	// step_delay = 1/tt * sqrt(2*alpha/accel)
	// step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
	m.step_delay = (T1_FREQ_148 * MATH_sqrt(A_SQ / accel))/100;
	
	// Find out after how many steps does the speed hit the max speed limit.
	// max_s_lim = speed^2 / (2*alpha*accel)
	m.max_s_lim = (long)speed*speed/(long)(((long)A_x20000*accel)/100);
	// If we hit max speed limit before 0,5 step it will round to 0.
	// But in practice we need to move atleast 1 step to get any speed at all.
	if(m.max_s_lim == 0) {
		m.max_s_lim = 1;
	}
}

/*! This tells the motor to drive any number of steps 
 *  \param step  Number of steps to move (pos - CW, neg - CCW).
 */
void MOT_MoveSteps(int16_t steps) {
	// Set direction from sign on step value.
	if(steps < 0) {
		m.dir = CCW;
		steps = -steps;
	}
	else {
		m.dir = CW;
	}
	
	// If moving only 1 step.
	if(steps == 1) {
		m.accel_count = -1;
		m.state = MOT_FSM_DECEL;
		m.step_delay = 1000;
		running = TRUE;
		OCR1A = 10;
		// Run Timer/Counter 1 with prescaler = 8.
		//TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
	}
	else if(steps != 0) {		
		// Find out after how many steps we must start deceleration.
		// n1 = (n1+n2)decel / (accel + decel)
		m.accel_lim = ((long)steps*m.decel) / (m.accel+m.decel);
		// We must accelrate at least 1 step before we can start deceleration.
		if(m.accel_lim == 0) {
			m.accel_lim = 1;
		}
		
		// Use the limit we hit first to calc decel.
		if(m.accel_lim <= m.max_s_lim) {
			m.decel_val = m.accel_lim - steps;
		}
		else {
			m.decel_val = -((long)m.max_s_lim*m.accel)/m.decel;
		}
		
		// We must decelrate at least 1 step to stop.
		if(m.decel_val == 0) {
			m.decel_val = -1;
		}
		
		// Find step to start decleration.
		m.decel_start = steps + m.decel_val;
		
		// If the maximum speed is so low that we dont need to go via accelration state.
		if(m.step_delay <= m.min_delay) {
			m.step_delay = m.min_delay;
			m.state = MOT_FSM_RUN;
		}
		else {
			m.state = MOT_FSM_ACCEL;
		}
		
		// Reset counter.
		m.step_count = 0;
		m.rest = 0;
		m.accel_count = 0;
		running = TRUE;
		OCR1A = 10;
		// Set Timer/Counter to divide clock by 8
		//TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));		
	}
}


/*! \brief Timer/Counter1 Output Compare A Match Interrupt.
 *
 *  Timer/Counter1 Output Compare A Match Interrupt.
 *  Increments/decrements the position of the stepper motor
 *  exept after last position, when it stops.
 *  The \ref step_delay defines the period of this interrupt
 *  and controls the speed of the stepper motor.
 *  A new step delay is calculated to follow wanted speed profile
 *  on basis of accel/decel parameters.
 */

void MOT_Process(void)
{
  uint16_t new_step_delay;
  
  SER_AddData16(OCR1A);
  SER_SendPacket('D');
  //AS1_SendChar("\n");
  
  OCR1A = m.step_delay;

  switch(m.state) {
    case MOT_FSM_STOP:
      m.step_count = 0;
      m.rest = 0;
      // Stop Timer/Counter 1.
      //TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
      running = FALSE;
      break;

    case MOT_FSM_ACCEL:
      //sm_driver_StepCounter(srd.dir);
      m.step_count++;
      m.accel_count++;
      new_step_delay = m.step_delay - (((2 * (long)m.step_delay) + m.rest)/(4 * m.accel_count + 1));
      m.rest = ((2 * (long)m.step_delay)+m.rest)%(4 * m.accel_count + 1);
      // Chech if we should start decelration.
      if(m.step_count >= m.decel_start) {
        m.accel_count = m.decel_val;
        m.state = MOT_FSM_DECEL;
      }
      // Chech if we hitted max speed.
      else if(new_step_delay <= m.min_delay) {
        m.last_accel_delay = new_step_delay;
        new_step_delay = m.min_delay;
        m.rest = 0;
        m.state = MOT_FSM_RUN;
      }
      break;

    case MOT_FSM_RUN:
      //sm_driver_StepCounter(srd.dir);
      m.step_count++;
      new_step_delay = m.min_delay;
      // Chech if we should start decelration.
      if(m.step_count >= m.decel_start) {
        m.accel_count = m.decel_val;
        // Start decelration with same delay as accel ended with.
        new_step_delay = m.last_accel_delay;
        m.state = MOT_FSM_DECEL;
      }
      break;

    case MOT_FSM_DECEL:
      //sm_driver_StepCounter(srd.dir);
      m.step_count++;
      m.accel_count++;
      new_step_delay = m.step_delay - (((2 * (long)m.step_delay) + m.rest)/(4 * m.accel_count + 1));
      m.rest = ((2 * (long)m.step_delay)+m.rest)%(4 * m.accel_count + 1);
      // Check if we at last step
      if(m.accel_count >= 0){
        m.state = MOT_FSM_STOP;
      }
      break;
  }
  m.step_delay = new_step_delay;
}

/*! \brief Square root routine.
 *
 * sqrt routine 'grupe', from comp.sys.ibm.pc.programmer
 * Subject: Summary: SQRT(int) algorithm (with profiling)
 *    From: warwick@cs.uq.oz.au (Warwick Allison)
 *    Date: Tue Oct 8 09:16:35 1991
 *
 *  \param x  Value to find square root of.
 *  \return  Square root of x.
 */
static uint32_t MATH_sqrt(uint32_t x)
{
  register uint32_t xr;  // result register
  register uint32_t q2;  // scan-bit register
  register uint8_t f;    // flag (one bit)

  xr = 0;                     // clear result
  q2 = 0x40000000L;           // higest possible result bit
  do
  {
    if((xr + q2) <= x)
    {
      x -= xr + q2;
      f = 1;                  // set flag
    }
    else{
      f = 0;                  // clear flag
    }
    xr >>= 1;
    if(f){
      xr += q2;               // test flag
    }
  } while(q2 >>= 2);          // shift twice
  if(xr < x){
    return xr +1;             // add for rounding
  }
  else{
    return xr;
  }
}

/*! \brief Find minimum value.
 *
 *  Returns the smallest value.
 *
 *  \return  Min(x,y).
 */
uint16_t min(uint16_t x, uint16_t y)
{
  if(x < y){
    return x;
  }
  else{
    return y;
  }
}
