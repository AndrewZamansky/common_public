/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "I2S_nuc505_api.h"
#include "src/I2S_nuc505.h"


#define	MODULE_NAME						I2S_nuc505
#define	MODULE_IOCTL_FUNCTION			I2S_nuc505_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE	struct I2S_nuc505_cfg_t

#ifdef DT_DEV_MODULE

	#ifndef I2S_NUC505_DT_NUM_OF_WORDS_IN_BUFFER
		#error "I2S_NUC505_DT_NUM_OF_WORDS_IN_BUFFER should be defined"
	#endif

	#ifndef I2S_NUC505_DT_MASTER_OR_SLAVE_MODE
		#error "I2S_NUC505_DT_MASTER_OR_SLAVE_MODE should be defined"
	#endif

	#if	(I2S_NUC505_DT_MASTER_OR_SLAVE_MODE == I2S_NUC505_API_MASTER_MODE)
		#ifndef I2S_NUC505_DT_SAMPLE_RATE
			#error "I2S_NUC505_DT_SAMPLE_RATE should be defined"
		#endif
	#endif

	#ifndef I2S_NUC505_DT_NUM_OF_BYTES_IN_WORD
		#error "I2S_NUC505_DT_NUM_OF_BYTES_IN_WORD should be defined"
	#endif

	#ifndef I2S_NUC505_DT_CALLBACK_PDEV
		#error "I2S_NUC505_DT_CALLBACK_PDEV should be defined"
	#endif

	#ifndef I2S_NUC505_DT_TIMER_PDEV
		#error "I2S_NUC505_DT_TIMER_PDEV should be defined"
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(I2S_NUC505_DT_CALLBACK_PDEV) ;
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(I2S_NUC505_DT_TIMER_PDEV) ;
	#define STATIC_DEV_DATA_STRUCT									\
		{															\
			I2S_NUC505_DT_MASTER_OR_SLAVE_MODE	,					\
			I2S_NUC505_DT_NUM_OF_BYTES_IN_WORD	,					\
			P_TO_STATIC_DEVICE_INST(I2S_NUC505_DT_CALLBACK_PDEV),	\
			P_TO_STATIC_DEVICE_INST(I2S_NUC505_DT_TIMER_PDEV),	\
			I2S_NUC505_DT_SAMPLE_RATE ,								\
			I2S_NUC505_DT_NUM_OF_WORDS_IN_BUFFER					\
		}

#endif

#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef I2S_NUC505_DT_SAMPLE_RATE
#undef I2S_NUC505_DT_MASTER_OR_SLAVE_MODE
#undef I2S_NUC505_DT_TIMER_PDEV
#undef I2S_NUC505_DT_NUM_OF_WORDS_IN_BUFFER
#undef I2S_NUC505_DT_NUM_OF_BYTES_IN_WORD
#undef I2S_NUC505_DT_CALLBACK_PDEV
