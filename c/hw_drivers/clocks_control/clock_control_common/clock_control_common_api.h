
#ifndef _CLOCK_CONTROL_COMMON_API_H_
#define _CLOCK_CONTROL_COMMON_API_H_

#include "dev_management_api.h"
#include "clock_control_api.h"

/**********  define API  types ************/


/**********  define API  functions  ************/
void clock_control_common_api_get_parent_clock_rate(
								struct cfg_clk_t *cfg_clk, uint32_t *rate);

uint8_t common_clock_api_standard_clk_ioctls(struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2);

#endif
