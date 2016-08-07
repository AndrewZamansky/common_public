/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "I2S_nuc505_api.h"
#include "src/I2S_nuc505.h"

uint8_t I2S_nuc505_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME				I2S_nuc505
#define	MODULE_IOCTL_FUNCTION	I2S_nuc505_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE	I2S_nuc505_instance_t

#ifdef DT_DEV_MODULE

	#ifndef I2S_NUC505_DT_NUM_OF_WORDS_IN_BUFFER
		#error "I2S_NUC505_DT_NUM_OF_WORDS_IN_BUFFER should be defined"
	#endif

	#ifndef I2S_NUC505_DT_NUM_OF_BYTES_IN_WORD
		#error "I2S_NUC505_DT_NUM_OF_BYTES_IN_WORD should be defined"
	#endif

	#ifndef I2S_NUC505_DT_CALLBACK_PDEV
		#error "I2S_NUC505_DT_CALLBACK_PDEV should be defined"
	#endif



	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(I2S_NUC505_DT_CALLBACK_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT									\
		{															\
			P_TO_STATIC_DEVICE_INST(I2S_NUC505_DT_CALLBACK_PDEV),	\
			I2S_NUC505_DT_NUM_OF_WORDS_IN_BUFFER,					\
			I2S_NUC505_DT_NUM_OF_BYTES_IN_WORD						\
		}

#endif

#include "add_component.h"

#undef I2S_NUC505_DT_NUM_OF_WORDS_IN_BUFFER
#undef I2S_NUC505_DT_NUM_OF_BYTES_IN_WORD
#undef I2S_NUC505_DT_CALLBACK_PDEV
