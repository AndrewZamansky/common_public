
#ifndef _CLOCK_CONTROL_API_H_
#define _CLOCK_CONTROL_API_H_

#include "dev_management_api.h" // for device manager defines and typedefs

/**********  define API  types ************/

typedef void (*get_freq_func_t)(uint32_t *freq, uint32_t parent_freq);
typedef void (*set_freq_func_t)(uint32_t freq, uint32_t parent_freq);
typedef void (*set_parent_clk_func_t)(struct dev_desc_t *parent_clk);
typedef void (*enable_clk_func_t)();

struct cfg_clk_t {
	struct dev_desc_t  *default_parent_clk;
	get_freq_func_t  get_freq_func;
	set_freq_func_t  set_freq_func;
	set_parent_clk_func_t set_parent_clk_func;
	enable_clk_func_t enable_clk_func;
};


enum clk_ioctl_t {
	CLK_IOCTL_SET_FREQ = IOCTL_LAST_COMMON_IOCTL + 1,
	CLK_IOCTL_GET_FREQ,
	CLK_IOCTL_SET_PARENT,
	CLK_IOCTL_ENABLE,
	CLK_IOCTL_GET_ROOT_CLK,
};


/**********  define API  functions  ************/

#endif
