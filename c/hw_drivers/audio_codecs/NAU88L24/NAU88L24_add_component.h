/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "NAU88L24_api.h"
#include "src/NAU88L24.h"


#define	MODULE_NAME                      NAU88L24
#define	MODULE_IOCTL_FUNCTION            NAU88L24_ioctl
//#define	MODULE_CALLBACK_FUNCTION         NAU88L24_callback
#define MODULE_CONFIG_DATA_STRUCT_TYPE   struct NAU88L24_config_t

#ifdef DT_DEV_MODULE

	#ifndef  NAU88L24_DT_I2C_BUS_DEV
		#error "NAU88L24_DT_I2C_BUS_DEV should be defined"
	#endif

	#ifndef  NAU88L24_DT_MCLK_CLK_DEV
		#error "NAU88L24_DT_MCLK_CLK_DEV should be defined"
	#endif
	#define POINTER_TO_MCLK_CLK_DEV	 \
			P_TO_STATIC_DEVICE_INST(NAU88L24_DT_MCLK_CLK_DEV)

	#ifndef  NAU88L24_DT_FSCLK_CLK_DEV
		#error "NAU88L24_DT_FSCLK_CLK_DEV should be defined"
	#endif

	#ifndef  NAU88L24_DT_INPUT_TYPE
		#error "NAU88L24_DT_INPUT_TYPE should be defined"
	#else
		#if ( NAU88L24_DT_INPUT_TYPE == NAU88L24_Input_type_DMIC)
			#ifndef  NAU88L24_DT_NUM_OF_DMIC
				#error "NAU88L24_DT_NUM_OF_DMIC should be defined"
			#endif
		#else
			#undef NAU88L24_DT_NUM_OF_DMIC
			#define NAU88L24_DT_NUM_OF_DMIC 0
		#endif
	#endif

	#ifndef  NAU88L24_DT_OUTPUT_TYPE
		#error "NAU88L24_DT_OUTPUT_TYPE should be defined"
	#endif

	#define POINTER_TO_FSCLK_CLK_DEV	 \
			P_TO_STATIC_DEVICE_INST(NAU88L24_DT_FSCLK_CLK_DEV)

	#ifndef  NAU88L24_DT_I2C_DEV_ADDRESS
		#error "NAU88L24_DT_I2C_DEV_ADDRESS should be defined"
	#endif
	#ifndef  NAU88L24_DT_PCM_WORD_WIDTH
		#error "NAU88L24_DT_PCM_WORD_WIDTH should be defined"
	#endif
	#define _POINTER_TO_I2C_BUS_DEV     \
			P_TO_STATIC_DEVICE_INST(NAU88L24_DT_I2C_BUS_DEV)

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(APP_DT_I2S_DEV) ;
	#define STATIC_DEV_DATA_STRUCT   \
		{                            \
			POINTER_TO_MCLK_CLK_DEV,     \
			POINTER_TO_FSCLK_CLK_DEV,    \
			_POINTER_TO_I2C_BUS_DEV,     \
			NAU88L24_DT_I2C_DEV_ADDRESS, \
			NAU88L24_DT_PCM_WORD_WIDTH , \
			NAU88L24_DT_INPUT_TYPE,      \
			NAU88L24_DT_OUTPUT_TYPE,     \
			NAU88L24_DT_NUM_OF_DMIC      \
		}

#endif

#include "add_component.h"

#undef  NAU88L24_DT_I2C_BUS_DEV
#undef  _POINTER_TO_I2C_BUS_DEV
#undef  NAU88L24_DT_I2C_DEV_ADDRESS
#undef  NAU88L24_DT_PCM_WORD_WIDTH
#undef  NAU88L24_DT_MCLK_CLK_DEV
#undef  NAU88L24_DT_FSCLK_CLK_DEV
#undef  POINTER_TO_MCLK_CLK_DEV
#undef  POINTER_TO_FSCLK_CLK_DEV
#undef  NAU88L24_DT_INPUT_TYPE
#undef  NAU88L24_DT_OUTPUT_TYPE
#undef  NAU88L24_DT_NUM_OF_DMIC
