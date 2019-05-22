/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "I2S_i94xxx_api.h"
#include "src/I2S_i94xxx.h"

#define	MODULE_NAME                       I2S_i94xxx
#define	MODULE_IOCTL_FUNCTION             I2S_i94xxx_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE    struct I2S_i94xxx_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE   struct I2S_i94xxx_runtime_t

#ifdef DT_DEV_MODULE

	#ifndef I2S_I94XXX_DT_MASTER_OR_SLAVE_MODE
		#error "I2S_I94XXX_DT_MASTER_OR_SLAVE_MODE should be defined"
	#endif

	#if	(I2S_I94XXX_DT_MASTER_OR_SLAVE_MODE == I2S_I94XXX_API_MASTER_MODE)
		#ifndef I2S_I94XXX_DT_SAMPLE_RATE
			#error "I2S_I94XXX_DT_SAMPLE_RATE should be defined"
		#endif
	#endif

	#ifndef I2S_I94XXX_DT_NUM_OF_BYTES_IN_WORD
		#error "I2S_I94XXX_DT_NUM_OF_BYTES_IN_WORD should be defined"
	#endif

	#ifndef I2S_I94XXX_DT_NUM_OF_BYTES_IN_WORD
		#error "I2S_I94XXX_DT_NUM_OF_BYTES_IN_WORD should be defined"
	#endif

	#ifndef I2S_I94XXX_DT_ENABLE_CHANNEL_REORDERING
		#define I2S_I94XXX_DT_ENABLE_CHANNEL_REORDERING  0
	#endif

	#ifndef I2S_I94XXX_DT_I2S_FORMAT
		#error "I2S_I94XXX_DT_I2S_FORMAT should be defined"
	#else
		#if ( I2S_I94XXX_FORMAT_I2S != I2S_I94XXX_DT_I2S_FORMAT)
			#ifndef I2S_I94XXX_DT_TDM_CH
				#error "I2S_I94XXX_DT_TDM_CH should be defined"
			#endif
			#undef  I2S_I94XXX_DT_STEREO_OR_MONO
			#define  I2S_I94XXX_DT_STEREO_OR_MONO   \
										I2S_I94XXX_API_AUDIO_FORMAT_STEREO
		#else
			#ifndef I2S_I94XXX_DT_STEREO_OR_MONO
				#error "I2S_I94XXX_DT_STEREO_OR_MONO should be defined"
			#endif
			#undef  I2S_I94XXX_DT_TDM_CH
			#if ( I2S_I94XXX_API_AUDIO_FORMAT_MONO == \
										I2S_I94XXX_DT_STEREO_OR_MONO)
				#define I2S_I94XXX_DT_TDM_CH  1
			#else
				#define I2S_I94XXX_DT_TDM_CH  2
			#endif
		#endif
	#endif

	#ifndef I2S_I94XXX_DT_MCLK_FACTOR_BASED_ON_FSCLK
		#error "I2S_I94XXX_DT_MCLK_FACTOR_BASED_ON_FSCLK should be defined"
	#endif

	#ifndef I2S_I94XXX_DT_BCLK_PIN
		#error "I2S_I94XXX_DT_BCLK_PIN should be defined"
	#endif

	#ifndef I2S_I94XXX_DT_LRCLK_PIN
		#error "I2S_I94XXX_DT_LRCLK_PIN should be defined"
	#endif


	#ifndef I2S_I94XXX_DT_DI_PIN
		#error "I2S_I94XXX_DT_DI_PIN should be defined"
	#endif

	#ifndef I2S_I94XXX_DT_DO_PIN
		#error "I2S_I94XXX_DT_DO_PIN should be defined"
	#endif

	#ifndef I2S_I94XXX_DT_MCLK_PIN
		#define I2S_I94XXX_DT_MCLK_PIN    0xffffffff
	#endif

	#ifndef I2S_I94XXX_DT_SRC_CLOCK_PDEV
		#error "I2S_I94XXX_DT_SRC_CLOCK_PDEV should be defined"
	#endif
	#define POINTER_TO_SRC_CLOCK_PDEV	 \
			P_TO_STATIC_DEVICE_INST(I2S_I94XXX_DT_SRC_CLOCK_PDEV)



	#define STATIC_DEV_DATA_STRUCT               \
		{                                        \
			POINTER_TO_SRC_CLOCK_PDEV,           \
			I2S_I94XXX_DT_MASTER_OR_SLAVE_MODE,  \
			I2S_I94XXX_DT_SAMPLE_RATE,           \
			I2S_I94XXX_DT_I2S_FORMAT,            \
			I2S_I94XXX_DT_STEREO_OR_MONO,        \
			I2S_I94XXX_DT_MCLK_FACTOR_BASED_ON_FSCLK, \
			I2S_I94XXX_DT_TDM_CH,                \
			I2S_I94XXX_DT_BCLK_PIN,              \
			I2S_I94XXX_DT_LRCLK_PIN,             \
			I2S_I94XXX_DT_DI_PIN,                \
			I2S_I94XXX_DT_DO_PIN,                \
			I2S_I94XXX_DT_MCLK_PIN,              \
			I2S_I94XXX_DT_NUM_OF_BYTES_IN_WORD,  \
			I2S_I94XXX_DT_ENABLE_CHANNEL_REORDERING, \
		}

#endif

#include "add_component.h"

/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef POINTER_TO_SRC_CLOCK_PDEV
#if	(I2S_I94XXX_DT_MASTER_OR_SLAVE_MODE == I2S_I94XXX_API_MASTER_MODE)
    #undef I2S_I94XXX_DT_SAMPLE_RATE
#endif
#undef I2S_I94XXX_DT_MASTER_OR_SLAVE_MODE
#undef I2S_I94XXX_DT_NUM_OF_BYTES_IN_WORD
#undef I2S_I94XXX_DT_DI_PIN
#undef I2S_I94XXX_DT_DO_PIN
#undef I2S_I94XXX_DT_MCLK_PIN
#undef I2S_I94XXX_DT_BCLK_PIN
#undef I2S_I94XXX_DT_LRCLK_PIN
#undef I2S_I94XXX_DT_I2S_FORMAT
#undef I2S_I94XXX_DT_TDM_CH
#undef I2S_I94XXX_DT_STEREO_OR_MONO
#undef I2S_I94XXX_DT_ENABLE_CHANNEL_REORDERING
#undef I2S_I94XXX_DT_MCLK_FACTOR_BASED_ON_FSCLK
