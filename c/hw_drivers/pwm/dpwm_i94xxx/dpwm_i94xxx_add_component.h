/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "dpwm_i94xxx_api.h"
#include  "src/dpwm_i94xxx.h"

#define	MODULE_NAME                     dpwm_i94xxx
#define	MODULE_IOCTL_FUNCTION           dpwm_i94xxx_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE  struct dpwm_i94xxx_cfg_t

#ifdef DT_DEV_MODULE

	#ifndef DPWM_I94XXX_DT_SAMPLE_RATE
		#error "DPWM_I94XXX_DT_SAMPLE_RATE should be defined"
	#endif

	#ifndef DPWM_I94XXX_DT_LEFT_CHANNEL_IO_BANK
		#error "DPWM_I94XXX_DT_LEFT_CHANNEL_IO_BANK should be defined"
	#endif
	#ifndef DPWM_I94XXX_DT_RIGHT_CHANNEL_IO_BANK
		#error "DPWM_I94XXX_DT_RIGHT_CHANNEL_IO_BANK should be defined"
	#endif
	#ifndef DPWM_I94XXX_DT_SUB_CHANNEL_IO_BANK
		#error "DPWM_I94XXX_DT_SUB_CHANNEL_IO_BANK should be defined"
	#endif

	#ifndef DPWM_I94XXX_DT_SRC_CLOCK_PDEV
		#error "DPWM_I94XXX_DT_SRC_CLOCK_PDEV should be defined"
	#endif
	#define POINTER_TO_SRC_CLOCK_PDEV  \
			P_TO_STATIC_DEVICE_INST(DPWM_I94XXX_DT_SRC_CLOCK_PDEV)



	#define STATIC_DEV_DATA_STRUCT      \
		{                               \
			POINTER_TO_SRC_CLOCK_PDEV,  \
			DPWM_I94XXX_DT_SAMPLE_RATE, \
			DPWM_I94XXX_DT_LEFT_CHANNEL_IO_BANK,  \
			DPWM_I94XXX_DT_RIGHT_CHANNEL_IO_BANK, \
			DPWM_I94XXX_DT_SUB_CHANNEL_IO_BANK    \
		}

#endif

#include "add_component.h"

#undef DPWM_I94XXX_DT_SAMPLE_RATE
#undef DPWM_I94XXX_DT_LEFT_CHANNEL_IO_BANK
#undef DPWM_I94XXX_DT_RIGHT_CHANNEL_IO_BANK
#undef DPWM_I94XXX_DT_SUB_CHANNEL_IO_BANK
#undef DPWM_I94XXX_DT_SRC_CLOCK_PDEV
#undef POINTER_TO_SRC_CLOCK_PDEV
