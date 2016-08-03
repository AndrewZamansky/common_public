/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "clock_control_nuc505_api.h"
#include 	"src/clock_control_nuc505.h"

uint8_t clock_control_nuc505_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME				clock_control_nuc505
#define	MODULE_IOCTL_FUNCTION	clock_control_nuc505_ioctl
#define MODULE_DATA_STRUCT_TYPE	CLOCK_CONTROL_NUC505_Instance_t

#ifdef DT_DEV_MODULE

	#ifndef CLOCK_CONTROL_NUC505_DT_XTAL_RATE
		#define CLOCK_CONTROL_NUC505_DT_XTAL_RATE	12000000
	#endif

	#ifndef CLOCK_CONTROL_NUC505_DT_CORE_RATE
		#define CLOCK_CONTROL_NUC505_DT_CORE_RATE	96000000
	#endif


	#define STATIC_DEV_DATA_STRUCT 	{	\
			{	\
				CLOCK_CONTROL_NUC505_DT_XTAL_RATE ,	\
				CLOCK_CONTROL_NUC505_DT_CORE_RATE	\
			}	\
		}
#endif

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef CLOCK_CONTROL_NUC505_DT_XTAL_RATE
#undef CLOCK_CONTROL_NUC505_DT_CORE_RATE