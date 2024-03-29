/**
 * \file
 * \brief Math function implementations.
 * \author Christoph Bächler
 *
 * This module implements different math functions. This is done to 
 * speed up calculation by using fixed point aritmetics instead of 
 * float or double calculations (in sqrt function for example).
 */
#include "PE_Types.h"
#include "Math.h"

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
uint32_t MATH_sqrt(uint32_t x) {
	register uint32_t xr;  // result register
	register uint32_t q2;  // scan-bit register
	register uint8_t f;    // flag (one bit)
	
	xr = 0;                     // clear result
	q2 = 0x40000000L;           // higest possible result bit
	do {
		if((xr + q2) <= x) {
			x -= xr + q2;
			f = 1;                  // set flag
		}
		else {
			f = 0;                  // clear flag
		}
		xr >>= 1;
		
		if(f) {
			xr += q2;               // test flag
		}
	} while(q2 >>= 2);          // shift twice
	if(xr < x) {
		return xr + 1;             // add for rounding
	}
	else {
		return xr;
	}
}

/*! \brief Find minimum value.
 *
 *  Returns the smaller value.
 *  \param x Value y
 *  \param y Value x
 *  \return  Min(x,y).
 */
uint16_t MATH_min(uint16_t x, uint16_t y) {
	if(x < y) {
		return x;
	}
	else {
		return y;
	}
}

/*! \brief Find maximum value.
 *
 *  Returns the bigger value.
 *  \param x Value y
 *  \param y Value x
 *  \return  Min(x,y).
 */
uint16_t MATH_max(uint16_t x, uint16_t y) {
	if(x > y) {
		return x;
	}
	else {
		return y;
	}
}