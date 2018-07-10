/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "I2S_onSPI_i94xxx_api.h"
#include "src/I2S_onSPI_i94xxx.h"

#define	MODULE_NAME						I2S_onSPI_i94xxx
#define	MODULE_IOCTL_FUNCTION			I2S_onSPI_i94xxx_ioctl
#define MODULE_CALLBACK_FUNCTION		I2S_onSPI_i94xxx_callback
#define MODULE_CONFIG_DATA_STRUCT_TYPE	struct I2S_onSPI_i94xxx_cfg_t
#define MODULE_RUNTIME_DATA_STRUCT_TYPE	struct I2S_onSPI_i94xxx_runtime_t

#ifdef DT_DEV_MODULE

	#ifndef I2S_onSPI_I94XXX_DT_MASTER_OR_SLAVE_MODE
		#error "I2S_onSPI_I94XXX_DT_MASTER_OR_SLAVE_MODE should be defined"
	#endif

	#if	(I2S_onSPI_I94XXX_DT_MASTER_OR_SLAVE_MODE == I2S_onSPI_I94XXX_API_MASTER_MODE)
		#ifndef I2S_onSPI_I94XXX_DT_SAMPLE_RATE
			#error "I2S_onSPI_I94XXX_DT_SAMPLE_RATE should be defined"
		#endif
	#endif

	#ifndef I2S_onSPI_I94XXX_DT_DATA_WIDTH
		#error "I2S_onSPI_I94XXX_DT_DATA_WIDTH should be defined"
	#endif

	#ifndef I2S_onSPI_I94XXX_DT_BASE_ADDRESS
		#error "I2S_onSPI_I94XXX_DT_BASE_ADDRESS should be defined"
	#endif

	#ifndef I2S_onSPI_I94XXX_DT_SRC_CLOCK_PDEV
		#error "I2S_onSPI_I94XXX_DT_SRC_CLOCK_PDEV should be defined"
	#endif

	#ifndef I2S_onSPI_I94XXX_DT_AUDIO_FORMAT
		#error "I2S_onSPI_I94XXX_DT_AUDIO_FORMAT should be defined"
	#endif

	#ifndef I2S_onSPI_I94XXX_DT_TXRX_FORMAT
		#error "I2S_onSPI_I94XXX_DT_TXRX_FORMAT should be defined"
	#endif

	#ifndef I2S_onSPI_I94XXX_DT_BCLK_PIN
		#error "I2S_onSPI_I94XXX_DT_BCLK_PIN should be defined"
	#endif

	#ifndef I2S_onSPI_I94XXX_DT_LRCLK_PIN
		#error "I2S_onSPI_I94XXX_DT_LRCLK_PIN should be defined"
	#endif

	#ifndef I2S_onSPI_I94XXX_DT_DI_PIN
		#error "I2S_onSPI_I94XXX_DT_DI_PIN should be defined"
	#endif

	#ifndef I2S_onSPI_I94XXX_DT_DO_PIN
		#error "I2S_onSPI_I94XXX_DT_DO_PIN should be defined"
	#endif

	#ifndef I2S_onSPI_I94XXX_DT_MCLK_PIN
		#warning "I2S_onSPI_I94XXX_DT_MCLK_PIN is not defined"
		#define I2S_onSPI_I94XXX_DT_MCLK_PIN  0
	#endif

	#define POINTER_TO_SRC_CLOCK_PDEV	 \
			P_TO_STATIC_DEVICE_INST(I2S_onSPI_I94XXX_DT_SRC_CLOCK_PDEV)



	#define STATIC_DEV_DATA_STRUCT						\
		{												\
			POINTER_TO_SRC_CLOCK_PDEV,			        \
			I2S_onSPI_I94XXX_DT_BASE_ADDRESS,			\
			I2S_onSPI_I94XXX_DT_MASTER_OR_SLAVE_MODE,	\
			I2S_onSPI_I94XXX_DT_SAMPLE_RATE,			\
			I2S_onSPI_I94XXX_DT_DATA_WIDTH,				\
			I2S_onSPI_I94XXX_DT_AUDIO_FORMAT,			\
			I2S_onSPI_I94XXX_DT_TXRX_FORMAT,			\
			I2S_onSPI_I94XXX_DT_BCLK_PIN,               \
			I2S_onSPI_I94XXX_DT_LRCLK_PIN,              \
			I2S_onSPI_I94XXX_DT_DI_PIN,                 \
			I2S_onSPI_I94XXX_DT_DO_PIN,                 \
			I2S_onSPI_I94XXX_DT_MCLK_PIN                \
		}

#endif

#include "add_component.h"

#undef POINTER_TO_SRC_CLOCK_PDEV
#if	(I2S_onSPI_I94XXX_DT_MASTER_OR_SLAVE_MODE == I2S_onSPI_I94XXX_API_MASTER_MODE)
    #undef I2S_onSPI_I94XXX_DT_SAMPLE_RATE
#endif
#undef I2S_onSPI_I94XXX_DT_MASTER_OR_SLAVE_MODE
#undef I2S_onSPI_I94XXX_DT_DATA_WIDTH
#undef I2S_onSPI_I94XXX_DT_BASE_ADDRESS
#undef I2S_onSPI_I94XXX_DT_AUDIO_FORMAT
#undef I2S_onSPI_I94XXX_DT_TXRX_FORMAT
#undef I2S_onSPI_I94XXX_DT_BCLK_PIN
#undef I2S_onSPI_I94XXX_DT_LRCLK_PIN
#undef I2S_onSPI_I94XXX_DT_DI_PIN
#undef I2S_onSPI_I94XXX_DT_DO_PIN
#undef I2S_onSPI_I94XXX_DT_MCLK_PIN
