/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "clock_control_xtensa_sim_api.h"
#include 	"src/clock_control_xtensa_sim.h"

#define	MODULE_NAME						clock_control_xtensa_sim
#define	MODULE_IOCTL_FUNCTION			clock_control_xtensa_sim_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE	struct clk_cntl_xtensa_sim_cfg_t

#ifdef DT_DEV_MODULE

	#define STATIC_DEV_DATA_STRUCT 	{		\
		0									\
		}
#endif

#include "add_component.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_component.h"
 */
#undef CLOCK_CONTROL_XTENSA_SIM_DT_PLL_RATE
#undef CLOCK_CONTROL_XTENSA_SIM_DT_HCLK_RATE
