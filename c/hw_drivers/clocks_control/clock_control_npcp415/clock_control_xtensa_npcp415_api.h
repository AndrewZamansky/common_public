
#ifndef _CLOCK_CONTROL_XTENSA_NPCP415_API_H_
#define _CLOCK_CONTROL_XTENSA_NPCP415_API_H_

#include "dev_management_api.h"
#include "clock_control_api.h"

extern struct dev_desc_t  *xtensa_npcp415_core_clk_dev;

struct clock_control_xtensa_npcp415_cfg_t {
	uint32_t           npcp415_core_rate;
};
SET_CONFIG_TYPE(
	clock_control_xtensa_npcp415, struct clock_control_xtensa_npcp415_cfg_t);

#endif
