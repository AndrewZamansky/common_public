/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "clock_control_nuc505_api.h"
#include 	"src/clock_control_nuc505.h"

#define	MODULE_NAME						clock_control_nuc505
#define	MODULE_IOCTL_FUNCTION			clock_control_nuc505_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE	struct clk_cntl_nuc505_cfg_t

#ifdef DT_DEV_MODULE

	#ifndef CLOCK_CONTROL_NUC505_DT_CORE_RATE
		#define CLOCK_CONTROL_NUC505_DT_CORE_RATE	96000000
	#endif


	#define STATIC_DEV_DATA_STRUCT 	{				\
				CLOCK_CONTROL_NUC505_DT_CORE_RATE	\
			}

#endif

#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef CLOCK_CONTROL_NUC505_DT_CORE_RATE
