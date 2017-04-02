
#ifndef _CLOCK_CONTROL_API_H_
#define _CLOCK_CONTROL_API_H_

#include "dev_management_api.h" // for device manager defines and typedefs

/**********  define API  types ************/

struct cfg_clk_t {
	uint32_t	rate;
	struct dev_desc_t   *parent_clk;
};


enum clk_ioctl_t {
	CLK_IOCTL_SET_FREQ = IOCTL_LAST_COMMON_IOCTL + 1,
	CLK_IOCTL_GET_FREQ,
	CLK_IOCTL_SET_PARENT,
	CLK_IOCTL_ENABLE
};

typedef enum
{
	IOCTL_CLOCK_CONTROL_GET_RATE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_CLOCK_CONTROL_SET_RATE,
}CLOCK_CONTROL_COMMON_API_ioctl_t;


/**********  define API  functions  ************/

#endif
