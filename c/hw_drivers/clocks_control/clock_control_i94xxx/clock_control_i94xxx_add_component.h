/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "clock_control_i94xxx_api.h"
#include 	"src/clock_control_i94xxx.h"

#define	MODULE_NAME						clock_control_i94xxx
#define	MODULE_IOCTL_FUNCTION			clock_control_i94xxx_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE	struct clk_cntl_i94xxx_cfg_t

#ifdef DT_DEV_MODULE

	#ifndef CLOCK_CONTROL_I94XXX_DT_PLL_RATE
		#error "CLOCK_CONTROL_I94XXX_DT_PLL_RATE should be defined"
	#endif

	#ifndef CLOCK_CONTROL_I94XXX_DT_HCLK_RATE
		#error "CLOCK_CONTROL_I94XXX_DT_HCLK_RATE should be defined"
	#endif

	#define STATIC_DEV_DATA_STRUCT 	{		\
		CLOCK_CONTROL_I94XXX_DT_PLL_RATE,	\
		CLOCK_CONTROL_I94XXX_DT_HCLK_RATE	\
		}
#endif

#include "add_component.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_component.h"
 */
#undef CLOCK_CONTROL_I94XXX_DT_PLL_RATE
#undef CLOCK_CONTROL_I94XXX_DT_HCLK_RATE
