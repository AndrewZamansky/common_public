/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "LM35_api.h"
#include "src/LM35.h"

uint8_t LM35_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME							LM35
#define	MODULE_IOCTL_FUNCTION				LM35_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE		LM35_Instance_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE		LM35_runtime_instance_t

#ifdef DT_DEV_MODULE

	#ifndef LM35_ADC_SERVER_PDEV
		#error "LM35_ADC_SERVER_PDEV should be defined"
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(LM35_ADC_SERVER_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT	\
		{											\
			P_TO_STATIC_DEVICE_INST(LM35_ADC_SERVER_PDEV)	\
		}
#endif

#include "add_static_dev.h"

#define MODULE_CONFIGURABLE_PARAMS_ARRAY	{	\
		{IOCTL_LM35_SET_SERVER_DEVICE , IOCTL_VOID , LM35_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},	\
	}

/* device specific defines should be undefined after calling #include "add_static_dev.h" */
#undef LM35_ADC_SERVER_PDEV
