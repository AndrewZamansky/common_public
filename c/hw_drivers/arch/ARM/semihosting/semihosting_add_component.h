/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "semihosting_api.h"
#include "src/semihosting.h"

extern uint8_t semihosting_ioctl(struct dev_desc_t *adev ,const uint8_t aIoctl_num , void * aIoctl_param1, void * aIoctl_param2);
extern size_t semihosting_pwrite(struct dev_desc_t *adev ,const uint8_t *apData , size_t aLength, size_t aOffset);

#define	MODULE_NAME				semihosting
#define	MODULE_IOCTL_FUNCTION	semihosting_ioctl
#define	MODULE_PWRITE_FUNCTION	semihosting_pwrite
#define MODULE_CONFIG_DATA_STRUCT_TYPE	SEMIHOSTING_Instance_t

#ifdef DT_DEV_MODULE

	#ifndef SEMIHOSTING_DT_CALLBACK_PDEV
		#error "SEMIHOSTING_DT_CALLBACK_PDEV should be defined"
	#endif


	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SEMIHOSTING_DT_CALLBACK_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT	\
		{							\
			P_TO_STATIC_DEVICE_INST(SEMIHOSTING_DT_CALLBACK_PDEV) 	\
		}

#endif


#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef SEMIHOSTING_DT_DT_CALLBACK_PDEV
