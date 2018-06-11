/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include 	"clock_control_api.h"

#define MODULE_CONFIG_DATA_STRUCT_TYPE   struct cfg_clk_t

#ifndef CLK_DT_IOCTL
	#error "CLK_DT_IOCTL should be defined"
#endif
#define MODULE_IOCTL_FUNCTION   CLK_DT_IOCTL

#ifdef DT_DEV_MODULE


	#ifndef CLK_DT_DEFAULT_RATE
		#define CLK_DT_DEFAULT_RATE    0
	#endif

	#ifndef CLK_DT_DEFAULT_PARENT
		#define CLK_DEFAULT_PARENT     NULL
	#else
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(CLK_DT_DEFAULT_PARENT) ;
		#define CLK_DEFAULT_PARENT        \
				P_TO_STATIC_DEVICE_INST(CLK_DT_DEFAULT_PARENT)
	#endif

	#define STATIC_DEV_DATA_STRUCT  {  \
		CLK_DT_DEFAULT_RATE,           \
		CLK_DEFAULT_PARENT             \
		}
#else
	#ifndef CLK_MODULE_NAME
		#error "CLK_MODULE_NAME should be defined"
	#endif
	#define MODULE_NAME  CLK_MODULE_NAME
#endif

#include "add_component.h"

/*
 * device specific defines should be undefined after
 *  calling #include "add_component.h"
 */
#undef CLK_DEFAULT_PARENT
#undef CLK_DT_IOCTL
#undef CLK_DT_DEFAULT_RATE
#undef CLK_DT_DEFAULT_PARENT
