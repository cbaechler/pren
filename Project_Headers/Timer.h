/**
 * \file
 * \brief Timer Module interface.
 * \author Christoph Bächler
 * \date 12.11.2013
 */

#ifndef TIMER_H_
#define TIMER_H_

#define TMR_TICK_MS 1

/*! \brief Function called from timer interrupt every TMR_TICK_MS. */
void TMR_OnInterrupt(void);

#endif /* TIMER_H_ */
