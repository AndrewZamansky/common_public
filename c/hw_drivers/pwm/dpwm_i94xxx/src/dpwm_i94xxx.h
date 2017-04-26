/*
 *
 * dpwm_i94xxx.h
 *
 *
 *
 *
 *
 */

#ifndef _DPWM_I94XXX_H_
#define _DPWM_I94XXX_H_

/********  includes *********************/

#include "src/_dpwm_i94xxx_prerequirements_check.h"

/********  types  *********************/

struct dpwm_i94xxx_cfg_t {
	struct dev_desc_t *   src_clock;
	uint32_t	sample_rate;
} ;

#endif
