/*
 *
 *clock_control_nuc505.h
 *
 *
 */

#ifndef _CLOCK_CONTROL_NUC505_H_
#define _CLOCK_CONTROL_NUC505_H_

/********  includes *********************/


/********  types  *********************/

typedef struct {
	uint32_t initial_clock_rates[NUC505_TOTAL_NUM_OF_CLOCKS];
} clock_control_nuc505_instance_t;

#endif
