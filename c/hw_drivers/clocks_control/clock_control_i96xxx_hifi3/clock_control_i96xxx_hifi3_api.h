
#ifndef _CLOCK_CONTROL_I96XXX_HIFI3_API_H_
#define _CLOCK_CONTROL_I96XXX_HIFI3_API_H_

#include "dev_management_api.h"
#include "clock_control_api.h"

/**********  define API  types ************/


/**********  define API  functions  ************/

extern struct dev_desc_t  *i96xxx_hifi3_dsp_clk_dev;

struct clk_cntl_i96xxx_hifi3_cfg_t {
	uint32_t           dsp_rate;
};

#endif
