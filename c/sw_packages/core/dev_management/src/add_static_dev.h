/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "dev_management_api.h"
#include "arch.h"


#ifndef DT_DEV_MODULE
	#error "DT_DEV_MODULE should be defined"
#endif

#ifndef DT_DEV_NAME
	#error "DT_DEV_NAME should be defined"
#endif

#if (defined(CONFIG_DYNAMIC_DEVICE_TREE) ||  \
		defined(CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES) || \
		(CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0))
	#ifndef MODULE_NAME
		#error "MODULE_NAME should be defined"
	#endif
#endif

#define DEV_CONFIG_DATA_INST(pdev)	DEV_CONFIG_DATA_INST2(pdev)
#define DEV_CONFIG_DATA_INST2(pdev)	config_data_inst_##pdev

#define DEV_RUNTIME_DATA_INST(pdev)	DEV_RUNTIME_DATA_INST2(pdev)
#define DEV_RUNTIME_DATA_INST2(pdev)	runtime_data_inst_##pdev



#ifndef	MODULE_CONFIG_DATA_STRUCT_TYPE

	#define	HANDLE_TO_DEV_CONFIG_DATA_STRUCT		NULL

#else

	#ifndef STATIC_DEV_DATA_STRUCT
		#error "STATIC_DEV_DATA_STRUCT should be defined"
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(DT_DEV_NAME) ;
	MODULE_CONFIG_DATA_STRUCT_TYPE	 DEVICE_DATA_PLACEMENT	\
				DEV_CONFIG_DATA_INST(DT_DEV_NAME) = STATIC_DEV_DATA_STRUCT;

	#define	HANDLE_TO_DEV_CONFIG_DATA_STRUCT  &DEV_CONFIG_DATA_INST(DT_DEV_NAME)

#endif




#ifndef	MODULE_RUNTIME_DATA_STRUCT_TYPE

	#define	HANDLE_TO_DEV_RUNTIME_DATA_STRUCT		NULL

#else

	//because of  bug 53119 in gcc disable "-Wmissing-braces" warning
	#if (4 == __GNUC__)
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wmissing-braces"
	#endif

	MODULE_RUNTIME_DATA_STRUCT_TYPE	 DEVICE_DATA_PLACEMENT	\
								DEV_RUNTIME_DATA_INST(DT_DEV_NAME) = {0};

	#define	HANDLE_TO_DEV_RUNTIME_DATA_STRUCT		\
										&DEV_RUNTIME_DATA_INST(DT_DEV_NAME)

	//because of  bug 53119 in gcc disable "-Wmissing-braces" warning
	#if (4 == __GNUC__)
		#pragma GCC diagnostic pop
	#endif

#endif





#define	_DEV_NAME(pdev)		_DEV_NAME2(pdev)
#define	_DEV_NAME2(pdev)	pdev##_str

#define DT_DECLARE_DEV_NAME_STR(pdev)	DT_DECLARE_DEV_NAME_STR2(pdev)
#define DT_DECLARE_DEV_NAME_STR2(pdev)		\
	DEVICE_DATA_PLACEMENT char _DEV_NAME(pdev)[] = STRINGIFY(pdev)

#define	_MODULE_NAME(module,pdev)		_MODULE_NAME2(module,pdev)
#define	_MODULE_NAME2(module,pdev)		module##pdev##_str

#define DT_DECLARE_MODULE_NAME_STR(module,pdev)		\
										DT_DECLARE_MODULE_NAME_STR2(module,pdev)
#define DT_DECLARE_MODULE_NAME_STR2(module,pdev)		\
	DEVICE_DATA_PLACEMENT char _MODULE_NAME(module,pdev)[] = STRINGIFY(module)

#if !defined(CONFIG_DONT_USE_DEVICE_NAME_STRINGS)
	DT_DECLARE_DEV_NAME_STR(DT_DEV_NAME);
#endif

#if (defined(CONFIG_DYNAMIC_DEVICE_TREE) ||  \
		defined(CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES) || \
		(CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0))

	DT_DECLARE_MODULE_NAME_STR(DT_DEV_MODULE , DT_DEV_NAME);
	DEVICE_PLACEMENT struct dev_desc_t STATIC_DEVICE_INST(DT_DEV_NAME) =
	{
		HANDLE_TO_DEV_CONFIG_DATA_STRUCT,
		HANDLE_TO_DEV_RUNTIME_DATA_STRUCT,
		_MODULE_NAME(DT_DEV_MODULE , DT_DEV_NAME),
		_DEV_NAME(DT_DEV_NAME),
		MODULE_IOCTL_FUNCTION,
		MODULE_PWRITE_FUNCTION,
		MODULE_PREAD_FUNCTION,
		MODULE_CALLBACK_FUNCTION
	};

#else

	DEVICE_PLACEMENT struct dev_desc_t STATIC_DEVICE_INST(DT_DEV_NAME) =
		{
			HANDLE_TO_DEV_CONFIG_DATA_STRUCT,
			HANDLE_TO_DEV_RUNTIME_DATA_STRUCT,
#if !defined(CONFIG_DONT_USE_DEVICE_NAME_STRINGS)
			_DEV_NAME(DT_DEV_NAME),
#endif
			MODULE_IOCTL_FUNCTION,
			MODULE_PWRITE_FUNCTION,
			MODULE_PREAD_FUNCTION,
			MODULE_CALLBACK_FUNCTION
		};

#endif


#ifndef CONFIG_DYNAMIC_DEVICE_TREE
	/* dont put DEVICE_PLACEMENT here as it can be __attribute__(section)
	 * and it will put pointer itself into this section */
	struct dev_desc_t  * DT_DEV_NAME =	P_TO_STATIC_DEVICE_INST(DT_DEV_NAME);
#endif

#undef	DT_DEV_MODULE
#undef	DT_DEV_NAME
#undef	STATIC_DEV_DATA_STRUCT
#undef	HANDLE_TO_DEV_CONFIG_DATA_STRUCT
#undef	HANDLE_TO_DEV_RUNTIME_DATA_STRUCT
