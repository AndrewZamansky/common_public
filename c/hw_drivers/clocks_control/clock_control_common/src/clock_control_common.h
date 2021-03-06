/*
 *
 * clock_control_common.h
 *
 */

#ifndef _clock_control_common_H_
#define _clock_control_common_H_

#include "dev_management_api.h"


struct runtime_clk_t {
	uint32_t          rate;// if 0 then take rate from parent
	struct dev_desc_t  *parent_clk;
	uint8_t init_done;
};

SET_RUNTIME_DATA_TYPE(clk_cntl, struct runtime_clk_t);

#endif
