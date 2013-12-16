#include "PE_Types.h"
#include "AS1.h"
#include "Serial.h"
#include "Motors.h"

static uint32_t MATH_sqrt(uint32_t x);

bool running;
uint16_t OCR1A;	/* emulate atmel register */

MOT_FSMData m;



//! Cointains data for timer interrupt.
//speedRampData srd;

void MOT_Init(void) {
  m.state = STOP;
}

/*! \brief Move the stepper motor a given number of steps.
 *
 *  Makes the stepper motor move the given number of steps.
 *  It accelrate with given accelration up to maximum speed and decelerate
 *  with given deceleration so it stops at the given step.
 *  If accel/decel is to small and steps to move is to few, speed might not
 *  reach the max speed limit before deceleration starts.
 *
 *  \param step  Number of steps to move (pos - CW, neg - CCW).
 *  \param accel  Accelration to use, in 0.01*rad/sec^2.
 *  \param decel  Decelration to use, in 0.01*rad/sec^2.
 *  \param speed  Max speed, in 0.01*rad/sec.
 */
void speed_cntr_Move(int16_t step, uint16_t accel, uint16_t decel, uint16_t speed)
{
  //! Number of steps before we hit max speed.
  //uint16_t max_s_lim;
  //! Number of steps before we must start deceleration (if accel does not hit max speed).
  //uint16_t accel_lim;

  // Set direction from sign on step value.
  if(step < 0){
    m.dir = CCW;
    step = -step;
  }
  else{
    m.dir = CW;
  }

  // If moving only 1 step.
  if(step == 1){
    // Move one step...
    m.accel_count = -1;
    // ...in DECEL state.
    m.state = DECEL;
    // Just a short delay so main() can act on 'running'.
    m.step_delay = 1000;
    running = TRUE;
    OCR1A = 10;
    // Run Timer/Counter 1 with prescaler = 8.
    //TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
  }
  // Only move if number of steps to move is not zero.
  else if(step != 0){
    // Refer to documentation for detailed information about these calculations.

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
    if(m.max_s_lim == 0){
      m.max_s_lim = 1;
    }

    // Find out after how many steps we must start deceleration.
    // n1 = (n1+n2)decel / (accel + decel)
    m.accel_lim = ((long)step*decel) / (accel+decel);
    // We must accelrate at least 1 step before we can start deceleration.
    if(m.accel_lim == 0){
      m.accel_lim = 1;
    }

    // Use the limit we hit first to calc decel.
    if(m.accel_lim <= m.max_s_lim){
      m.decel_val = m.accel_lim - step;
    }
    else{
      m.decel_val = -((long)m.max_s_lim*accel)/decel;
    }
    // We must decelrate at least 1 step to stop.
    if(m.decel_val == 0){
      m.decel_val = -1;
    }

    // Find step to start decleration.
    m.decel_start = step + m.decel_val;

    // If the maximum speed is so low that we dont need to go via accelration state.
    if(m.step_delay <= m.min_delay){
      m.step_delay = m.min_delay;
      m.state = RUN;
    }
    else{
      m.state = ACCEL;
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

void speed_cntr_TIMER1_COMPA_interrupt( void )
{
  // Holds next delay period.
  uint16_t new_step_delay;
  // Remember the last step delay used when accelrating.
  //static int16_t last_accel_delay;
  // Counting steps when moving.
  //static uint16_t step_count = 0;
  // Keep track of remainder from new_step-delay calculation to incrase accurancy
  //static uint16_t rest = 0;

  //uint16_t tmp = OCR1A;
  
  //AS1_SendChar((uint8_t) (tmp >> 8));
  //AS1_SendChar((uint8_t) (tmp & 0xFF));
  
  SER_AddData16(OCR1A);
  SER_SendPacket('D');
  //AS1_SendChar("\n");
  
  OCR1A = m.step_delay;

  switch(m.state) {
    case STOP:
      m.step_count = 0;
      m.rest = 0;
      // Stop Timer/Counter 1.
      //TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
      running = FALSE;
      break;

    case ACCEL:
      //sm_driver_StepCounter(srd.dir);
      m.step_count++;
      m.accel_count++;
      new_step_delay = m.step_delay - (((2 * (long)m.step_delay) + m.rest)/(4 * m.accel_count + 1));
      m.rest = ((2 * (long)m.step_delay)+m.rest)%(4 * m.accel_count + 1);
      // Chech if we should start decelration.
      if(m.step_count >= m.decel_start) {
        m.accel_count = m.decel_val;
        m.state = DECEL;
      }
      // Chech if we hitted max speed.
      else if(new_step_delay <= m.min_delay) {
        m.last_accel_delay = new_step_delay;
        new_step_delay = m.min_delay;
        m.rest = 0;
        m.state = RUN;
      }
      break;

    case RUN:
      //sm_driver_StepCounter(srd.dir);
      m.step_count++;
      new_step_delay = m.min_delay;
      // Chech if we should start decelration.
      if(m.step_count >= m.decel_start) {
        m.accel_count = m.decel_val;
        // Start decelration with same delay as accel ended with.
        new_step_delay = m.last_accel_delay;
        m.state = DECEL;
      }
      break;

    case DECEL:
      //sm_driver_StepCounter(srd.dir);
      m.step_count++;
      m.accel_count++;
      new_step_delay = m.step_delay - (((2 * (long)m.step_delay) + m.rest)/(4 * m.accel_count + 1));
      m.rest = ((2 * (long)m.step_delay)+m.rest)%(4 * m.accel_count + 1);
      // Check if we at last step
      if(m.accel_count >= 0){
        m.state = STOP;
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
