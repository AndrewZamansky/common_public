
#ifndef _dev_management_api_h_
#define _dev_management_api_h_


#include "_project_typedefs.h"
#include "_project_defines.h"
#include "dev_common_ioctl_and_types.h"


#ifdef  __cplusplus
extern "C" {
#endif

#define NOT_FOR_SAVE    0
#define FOR_SAVE        1

#define DEV_MAGIC_NUMBER    0x1B3D
#define MODULE_MAGIC_NUMBER    0x1C3E


#if defined(CONFIG_USE_SPECIFIC_MEMORY_LOCATION_FOR_DEVICES) &&  \
	( (defined(CONFIG_CORTEX_M0) || defined(CONFIG_CORTEX_M3) || \
		defined(CONFIG_CORTEX_M4) || \
		defined(CONFIG_CORTEX_A9) || defined(CONFIG_CORTEX_A35) )  &&  \
		defined(CONFIG_GCC) )

	#define MODULES_PLACEMENT        __attribute__((section("modules_section")))
	#define DEVICE_PLACEMENT     __attribute__((section("static_devs_section")))
	#define DEVICE_DATA_PLACEMENT  __attribute__((section(".static_devs_data")))

#elif defined(CONFIG_XTENSA_XCC)

	#define MODULES_PLACEMENT   \
		__attribute__((section(".data.keepsort._A5B6_modules")))
	#define DEVICE_PLACEMENT_FIRST   \
		__attribute__((section(".data.keepsort._A5B6_static_devs_1")))
	#define DEVICE_PLACEMENT   \
		__attribute__((section(".data.keepsort._A5B6_static_devs_2")))
	#define DEVICE_PLACEMENT_LAST   \
		__attribute__((section(".data.keepsort._A5B6_static_devs_3")))
	#define DEVICE_DATA_PLACEMENT \
		__attribute__((section(".data.keepsort._A5B6_static_devs_data")))

#else
	#define MODULES_PLACEMENT

	#define DEVICE_PLACEMENT

	#define DEVICE_DATA_PLACEMENT
#endif


// Macro for adding quotes
#define STRINGIFY(X) STRINGIFY2(X)
#define STRINGIFY2(X) #X

#define MODULE_INST2(module)  module_inst_##module
#define MODULE_INST(module)   MODULE_INST2(module)

#define STATIC_DEVICE_INCLUDE_NAME(module_name)  \
								STATIC_DEVICE_INCLUDE_NAME2(module_name)

#define STATIC_DEVICE_INCLUDE_NAME2(module_name)  module_name##_add_device.h

#define ADD_CURRENT_DEV  \
						STRINGIFY(STATIC_DEVICE_INCLUDE_NAME(DT_DEV_MODULE))



#define EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(pdev) \
								EXTERN_DECLARATION_TO_STATIC_DEVICE_INST2(pdev)

#define EXTERN_DECLARATION_TO_STATIC_DEVICE_INST2(pdev) \
						extern DEVICE_PLACEMENT struct dev_desc_t inst_##pdev

#define STATIC_DEVICE_INST(pdev)        STATIC_DEVICE_INST2(pdev)
#define STATIC_DEVICE_INST2(pdev)       inst_##pdev
#define P_TO_STATIC_DEVICE_INST(pdev)   &STATIC_DEVICE_INST(pdev)

#define _DT_ARRAY_NAME3(arr_name, pdev)  arr_name##_##pdev
#define _DT_ARRAY_NAME2(arr_name, pdev)  _DT_ARRAY_NAME3(arr_name, pdev)
#define DT_ARRAY_NAME(arr_name)  _DT_ARRAY_NAME2(arr_name, DT_DEV_NAME)
#define DECLARE_DT_UINT8_ARRAY(arr_name, arr_values)  \
		DEVICE_PLACEMENT uint8_t   DT_ARRAY_NAME(arr_name)[] = arr_values;

#define DT_UINT8_ARRAY_SIZE(arr_name)  \
					sizeof(DT_ARRAY_NAME(arr_name))/sizeof(uint8_t)



#define MODULE_RUNTIME_DATA_TYPE2(module)   _module_##module##_runtime_data_t
#define MODULE_RUNTIME_DATA_TYPE(module)    MODULE_RUNTIME_DATA_TYPE2(module)
#define MODULE_CONFIG_DATA_TYPE2(module)  _module_##module##_config_t
#define MODULE_CONFIG_DATA_TYPE(module)   MODULE_CONFIG_DATA_TYPE2(module)

#define SET_CONFIG_TYPE(module, type)  \
				typedef type MODULE_CONFIG_DATA_TYPE(module)

#define DEV_CONFIG_DATA_INST(pdev)   DEV_CONFIG_DATA_INST2(pdev)
#define DEV_CONFIG_DATA_INST2(pdev)  config_data_inst_##pdev
// declare extern current device in case it's pointer used in config data
#define SET_STATIC_DEV_CONFIG(module) \
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(DT_DEV_NAME);\
	DEVICE_DATA_PLACEMENT  MODULE_CONFIG_DATA_TYPE(module)\
							DEV_CONFIG_DATA_INST(DT_DEV_NAME)

#define SET_RUNTIME_DATA_TYPE(module, type)  \
								typedef type MODULE_RUNTIME_DATA_TYPE(module)

typedef enum
{
	DEV_PARAM_TYPE_PDEVICE,
	DEV_PARAM_TYPE_UINT8  ,
	DEV_PARAM_TYPE_UINT16 ,
	DEV_PARAM_TYPE_INT    ,
	DEV_PARAM_TYPE_UINT32 ,
	DEV_PARAM_TYPE_STRING ,
	DEV_PARAM_TYPE_MAPPED_SET_TO_SIZE
}dev_param_types_t;


struct mapped_set_to_size_param_t {
	char     *nameStr;
	size_t   val;
};

struct dev_param_t {
	char               *paramNameStr;
	uint8_t            paramSetIoctl;
	uint8_t            paramGetIoctl;
	dev_param_types_t  param_type;
	void               *p_mapped_set_type_param;
	uint8_t            mapped_set_size;
};

#define MAPPED_SET_TO_SIZE_PARAM(mapped_set_array)  &mapped_set_array , \
		(sizeof(mapped_set_array) / sizeof(struct mapped_set_to_size_param_t))
#define MAPPED_SET_DUMMY_PARAM()   NULL , 0


enum params_status_e {
	PARAMS_STATUS_IDLE = 0,
	PARAMS_STATUS_CHANGED_AFTER_LAST_TEST,
	PARAMS_STATUS_NO_CHANGED_AFTER_LAST_TEST
};


struct dev_desc_t;// pre-declare dev_desc_t structure to avoid warnings

typedef uint8_t (*dev_ioctl_func_t)(struct dev_desc_t *adev,
		 uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2);
typedef size_t (*dev_pwrite_func_t)(struct dev_desc_t *adev,
				const   uint8_t *apData, size_t aLength, size_t aOffset);
typedef uint32_t (*dev_pwrite32_func_t)(struct dev_desc_t *adev,
					const uint8_t *apData, uint32_t aLength, uint32_t aOffset);
typedef size_t (*dev_pread_func_t)(struct dev_desc_t *adev,
							uint8_t *apData , size_t aLength, size_t aOffset);
typedef uint32_t (*dev_pread32_func_t)(struct dev_desc_t *adev,
						uint8_t *apData, uint32_t aLength, uint32_t aOffset);
typedef size_t (*dev_read_func_t)(struct dev_desc_t *adev,
									uint8_t *apData, size_t aLength);
typedef uint8_t (*dev_callback_func_t)(struct dev_desc_t *adev,
							uint8_t aCallback_num, void * aCallback_param1,
							void * aCallback_param2);


struct dev_desc_t {
#if defined(CONFIG_USE_DEVICE_NAME_STRINGS)
	char                 *dev_name;
#endif
	struct included_module_t *module;
	void*               p_config_data;
	void*               p_runtime_data;
#ifdef CONFIG_USE_SPECIFIC_MEMORY_LOCATION_FOR_DEVICES
	void                 *magic_number;
#endif
};



struct included_module_t {
	char                 *module_name;
	dev_ioctl_func_t     ioctl;
	dev_pwrite_func_t    pwrite;
	dev_pread_func_t     pread;
	dev_callback_func_t  callback;
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES
	struct dev_param_t   const *config_params_arr;
	uint8_t              size_of_config_params_arr;
#endif
	size_t              module_config_struct_size;
	size_t              module_runtime_struct_size;
	void                 *magic_number;
};


/*  get data functions
 * add 'module' parameter as protection. it will create module casting.
 * in the case that type of receiving variable will not match,
 * then we receive warning or error that needed to be fixed.
 */
#define DEV_GET_CONFIG_DATA_POINTER(module, dev)   \
		(MODULE_CONFIG_DATA_TYPE(module)*)\
			(((struct dev_desc_t *)dev)->p_config_data)
#define DEV_GET_RUNTIME_DATA_POINTER(module, dev)  \
	(MODULE_RUNTIME_DATA_TYPE(module)*)( \
			((struct dev_desc_t *)dev)->p_runtime_data)

/*  ioctl functions */
#define DEV_IOCTL        DEV_IOCTL_1_PARAMS
#define DEV_IOCTL_0_PARAMS(dev, ioctl_num)    \
				DEV_IOCTL_2_PARAMS(dev, ioctl_num, NULL, NULL)
#define DEV_IOCTL_1_PARAMS(dev, ioctl_num, ioctl_param1)    \
				DEV_IOCTL_2_PARAMS(dev, ioctl_num, ioctl_param1, NULL)
uint8_t DEV_IOCTL_2_PARAMS(struct dev_desc_t * dev,
						uint8_t ioctl_num, void *param1, void *param2);


/* callback functions */
#define DEV_CALLBACK_0_PARAMS(dev, callback_num)    \
				DEV_CALLBACK_2_PARAMS(dev, callback_num, NULL, NULL)
#define DEV_CALLBACK_1_PARAMS(dev, callback_num, param1)    \
				DEV_CALLBACK_2_PARAMS(dev, callback_num, param1, NULL)
uint8_t	DEV_CALLBACK_2_PARAMS(struct dev_desc_t *adev,
						uint8_t ioctl_num, void *param1, void *param2);


size_t	DEV_PWRITE(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset);
size_t	DEV_PWRITE32(struct dev_desc_t *adev,
			const uint8_t *apData, uint32_t aLength, uint32_t aOffset);
#define	DEV_WRITE(adev, apData, aLength)   \
					DEV_PWRITE(adev, apData, aLength, 0)

size_t	DEV_PREAD(struct dev_desc_t *adev,
			uint8_t *apData, size_t aLength, size_t aOffset);
size_t	DEV_PREAD32(struct dev_desc_t *adev,
			uint8_t *apData, uint32_t aLength, uint32_t aOffset);
#define	DEV_READ(adev, apData, aLength)   \
					DEV_PREAD(adev, apData, aLength, 0)

uint8_t DEV_API_dummy_ioctl_func( struct dev_desc_t *adev,
		uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2);
uint8_t DEV_API_dummy_callback_func( struct dev_desc_t *adev,
			 uint8_t aCallback_num, void * param1, void * param2);
size_t DEV_API_dummy_pread_func(struct dev_desc_t *adev,
				uint8_t *apData, size_t aLength, size_t aOffset);
size_t DEV_API_dummy_pwrite_func(struct dev_desc_t *adev,
				const uint8_t *apData , size_t aLength, size_t aOffset);
size_t DEV_API_dummy_init_func(struct dev_desc_t *aDevDescriptor);
struct dev_desc_t * DEV_OPEN(const char *device_name);
uint8_t DEV_SET_PARAM(char *dev_name_str,
				char *param_name_str, char *param_val_str);


struct dev_desc_t * DevManagment_API_GetAllDevsArray(void);


#ifdef  __cplusplus
}
#endif

#endif
