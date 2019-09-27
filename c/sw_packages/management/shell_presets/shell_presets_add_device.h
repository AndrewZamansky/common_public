/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "shell_presets_api.h"
#include "src/shell_presets.h"


#define MODULE_CONFIG_DATA_STRUCT_TYPE		struct shell_presets_cfg_t


#ifndef SHELL_PRESETS_DT_STORAGE_PDEV
	#error "SHELL_PRESETS_DT_STORAGE_PDEV should be defined"
#endif

#ifndef SHELL_PRESETS_DT_CALLBACK_PDEV
	#define _CALLBACK_PDEV	NULL
#else
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_PRESETS_DT_CALLBACK_PDEV);
	#define _CALLBACK_PDEV	\
			P_TO_STATIC_DEVICE_INST(SHELL_PRESETS_DT_CALLBACK_PDEV)
#endif

#ifndef SHELL_PRESETS_DT_PRESET_SIZE
	#error "SHELL_PRESETS_DT_PRESET_SIZE should be defined"
#endif



EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SHELL_PRESETS_DT_STORAGE_PDEV) ;
#define STATIC_DEV_DATA_STRUCT									\
	{															\
		P_TO_STATIC_DEVICE_INST(SHELL_PRESETS_DT_STORAGE_PDEV), \
		_CALLBACK_PDEV,											\
		SHELL_PRESETS_DT_PRESET_SIZE							\
	}



#include "add_static_dev.h"

/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef SHELL_PRESETS_DT_STORAGE_PDEV
#undef SHELL_PRESETS_DT_CALLBACK_PDEV
#undef _CALLBACK_PDEV
#undef SHELL_PRESETS_DT_PRESET_SIZE
