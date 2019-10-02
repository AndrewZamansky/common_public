/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "semihosting_api.h"
#include "src/semihosting.h"


#define	MODULE_NAME             semihosting
#define	MODULE_IOCTL_FUNCTION   semihosting_ioctl
#define	MODULE_PWRITE_FUNCTION  semihosting_pwrite
#define MODULE_CONFIG_DATA_STRUCT_TYPE struct semihosting_instance_t

#ifdef DT_DEV_MODULE

	#ifndef SEMIHOSTING_DT_CALLBACK_PDEV
		#define	__SEMIHOSTING_DT_CALLBACK_PDEV     NULL
	#else
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SEMIHOSTING_DT_CALLBACK_PDEV);
		#define __SEMIHOSTING_DT_CALLBACK_PDEV    \
					P_TO_STATIC_DEVICE_INST(SEMIHOSTING_DT_CALLBACK_PDEV)
	#endif

	#define STATIC_DEV_DATA_STRUCT          \
		{                                   \
			__SEMIHOSTING_DT_CALLBACK_PDEV \
		}

#endif


#include "add_static_dev.h"

/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef SEMIHOSTING_DT_CALLBACK_PDEV
#undef __SEMIHOSTING_DT_CALLBACK_PDEV
