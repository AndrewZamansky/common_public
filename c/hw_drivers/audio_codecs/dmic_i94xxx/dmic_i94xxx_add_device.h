/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "dmic_i94xxx_api.h"
#include  "src/dmic_i94xxx.h"

#define  MODULE_CONFIG_DATA_STRUCT_TYPE  struct dmic_i94xxx_cfg_t


#ifndef DMIC_I94XXX_DT_SAMPLE_RATE
	#error "DMIC_I94XXX_DT_BAUD_RATE should be defined"
#endif

#ifndef DMIC_I94XXX_DT_SRC_CLOCK_PDEV
	#error "DMIC_I94XXX_DT_SRC_CLOCK_PDEV should be defined"
#endif
#define POINTER_TO_SRC_CLOCK_PDEV     \
		P_TO_STATIC_DEVICE_INST(DMIC_I94XXX_DT_SRC_CLOCK_PDEV)

#ifndef DMIC_I94XXX_DT_NUM_OF_CHANNELS
	#error "DMIC_I94XXX_DT_NUM_OF_CHANNELS should be defined"
#endif

#ifndef DMIC_I94XXX_DT_DATA0_PIN
	#error "DMIC_I94XXX_DT_DATA0_PIN should be defined"
#endif

#ifndef DMIC_I94XXX_DT_CLK0_PIN
	#error "DMIC_I94XXX_DT_CLK0_PIN should be defined"
#endif

#ifndef DMIC_I94XXX_DT_DATA1_PIN
	#error "DMIC_I94XXX_DT_DATA1_PIN should be defined"
#endif

#ifndef DMIC_I94XXX_DT_CLK1_PIN
	#error "DMIC_I94XXX_DT_CLK1_PIN should be defined"
#endif

#define STATIC_DEV_DATA_STRUCT          \
	{                                   \
		POINTER_TO_SRC_CLOCK_PDEV   ,   \
		DMIC_I94XXX_DT_SAMPLE_RATE,     \
		DMIC_I94XXX_DT_NUM_OF_CHANNELS, \
		DMIC_I94XXX_DT_DATA0_PIN,       \
		DMIC_I94XXX_DT_CLK0_PIN,        \
		DMIC_I94XXX_DT_DATA1_PIN,       \
		DMIC_I94XXX_DT_CLK1_PIN         \
	}


#include "add_static_dev.h"

/*
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef DMIC_I94XXX_DT_NUM_OF_CHANNELS
#undef POINTER_TO_SRC_CLOCK_PDEV
#undef DMIC_I94XXX_DT_SAMPLE_RATE
#undef DMIC_I94XXX_DT_SRC_CLOCK_PDEV
#undef DMIC_I94XXX_DT_DATA0_PIN
#undef DMIC_I94XXX_DT_CLK0_PIN
#undef DMIC_I94XXX_DT_DATA1_PIN
#undef DMIC_I94XXX_DT_CLK1_PIN
