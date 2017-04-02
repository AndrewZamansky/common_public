/*
 *
 *clock_control_i94xxx.h
 *
 *
 *
 *
 *
 */

#ifndef _CLOCK_CONTROL_I94XXX_H_
#define _CLOCK_CONTROL_I94XXX_H_

/********  includes *********************/

#include "src/_cortexM_systick_prerequirements_check.h"

/********  types  *********************/

struct clk_cntl_i94xxx_cfg_t {
	uint32_t pll_rate;
	uint32_t hclk_rate;
} ;

#endif
