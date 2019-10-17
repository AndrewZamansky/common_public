/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "dev_management_api.h"
#include "arch.h"


#ifndef DT_DEV_MODULE
	#error "DT_DEV_MODULE should be defined"
#endif

#ifndef DT_DEV_NAME
	#error "DT_DEV_NAME should be defined"
#endif



#define DEV_RUNTIME_DATA_INST(pdev)   DEV_RUNTIME_DATA_INST2(pdev)
#define DEV_RUNTIME_DATA_INST2(pdev)  runtime_data_inst_##pdev



#ifdef MODULE_HAS_NO_CONFIG_DATA

	// set dummy type as protection. if it was already set, then we get warning
	// or error during compilation
	SET_CONFIG_TYPE(DT_DEV_MODULE, uint8_t);
	#define	HANDLE_TO_DEV_CONFIG_DATA_STRUCT    NULL

#else

	#define	HANDLE_TO_DEV_CONFIG_DATA_STRUCT  &DEV_CONFIG_DATA_INST(DT_DEV_NAME)

#endif




#ifdef MODULE_HAS_NO_RUNTIME_DATA

	// set dummy type as protection. if it was already set, then we get warning
	// or error during compilation
	SET_RUNTIME_DATA_TYPE(DT_DEV_MODULE, uint8_t);
	#define	HANDLE_TO_DEV_RUNTIME_DATA_STRUCT   NULL

#else

	//because of  bug 53119 in gcc disable "-Wmissing-braces" warning
	#if (4 == __GNUC__) && \
		!defined(CONFIG_XTENSA_XCC) && !defined(__ARMCC_VERSION)
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wmissing-braces"
	#endif

	MODULE_RUNTIME_DATA_TYPE(DT_DEV_MODULE)  DEVICE_DATA_PLACEMENT \
								DEV_RUNTIME_DATA_INST(DT_DEV_NAME) = {0};

	#define	HANDLE_TO_DEV_RUNTIME_DATA_STRUCT  \
								&DEV_RUNTIME_DATA_INST(DT_DEV_NAME)

	//because of  bug 53119 in gcc disable "-Wmissing-braces" warning
	#if (4 == __GNUC__) && \
		!defined(CONFIG_XTENSA_XCC) && !defined(__ARMCC_VERSION)
		#pragma GCC diagnostic pop
	#endif

#endif





#define	_DEV_NAME(pdev)   _DEV_NAME2(pdev)
#define	_DEV_NAME2(pdev)  pdev##_str

#define DT_DECLARE_DEV_NAME_STR(pdev)   DT_DECLARE_DEV_NAME_STR2(pdev)
#define DT_DECLARE_DEV_NAME_STR2(pdev)  \
	DEVICE_DATA_PLACEMENT char _DEV_NAME(pdev)[] = STRINGIFY(pdev)

#if defined(CONFIG_USE_DEVICE_NAME_STRINGS)
	DT_DECLARE_DEV_NAME_STR(DT_DEV_NAME);
#endif


extern struct included_module_t MODULE_INST(DT_DEV_MODULE);

DEVICE_PLACEMENT struct dev_desc_t STATIC_DEVICE_INST(DT_DEV_NAME) =
	{
#if defined(CONFIG_USE_DEVICE_NAME_STRINGS)
		_DEV_NAME(DT_DEV_NAME),
#endif
		&MODULE_INST(DT_DEV_MODULE),
		HANDLE_TO_DEV_CONFIG_DATA_STRUCT,
		HANDLE_TO_DEV_RUNTIME_DATA_STRUCT,
#ifdef CONFIG_USE_SPECIFIC_MEMORY_LOCATION_FOR_DEVICES
		(void*)DEV_MAGIC_NUMBER
#endif
	};



#ifndef CONFIG_DYNAMIC_DEVICE_TREE
	/* dont put DEVICE_PLACEMENT here as it can be __attribute__(section)
	 * and it will put pointer itself into this section */
	struct dev_desc_t  * DT_DEV_NAME =	P_TO_STATIC_DEVICE_INST(DT_DEV_NAME);
#endif


#undef  DT_DEV_MODULE
#undef  DT_DEV_NAME
#undef  HANDLE_TO_DEV_CONFIG_DATA_STRUCT
#undef  HANDLE_TO_DEV_RUNTIME_DATA_STRUCT
#undef  MODULE_HAS_NO_CONFIG_DATA
#undef  MODULE_HAS_NO_RUNTIME_DATA
