/*
 *
 * dpwm_i94xxx.h
 *
 *
 */

#ifndef _DPWM_I94XXX_H_
#define _DPWM_I94XXX_H_

/********  includes *********************/

#include "dev_management_api.h"

/********  types  *********************/

struct dpwm_i94xxx_cfg_t {
	struct dev_desc_t *   src_clock;
	uint32_t	sample_rate_hz;
	uint32_t	left_ch_p;
	uint32_t	left_ch_n;
	uint32_t	right_ch_p;
	uint32_t	right_ch_n;
	uint32_t	sub_ch_p;
	uint32_t	sub_ch_n;
};

#endif
