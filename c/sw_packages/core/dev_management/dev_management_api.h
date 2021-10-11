
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

enum dev_param_types_e {
	DEV_PARAM_TYPE_PDEVICE,
	DEV_PARAM_TYPE_UINT8  ,
	DEV_PARAM_TYPE_UINT16 ,
	DEV_PARAM_TYPE_INT    ,
	DEV_PARAM_TYPE_UINT32 ,
	DEV_PARAM_TYPE_STRING ,
	DEV_PARAM_TYPE_MAPPED_SET_TO_SIZE
};


struct mapped_set_to_size_param_t {
	char     *nameStr;
	size_t   val;
};

struct dev_param_t {
	char               *paramNameStr;
	uint8_t            paramSetIoctl;
	uint8_t            paramGetIoctl;
	enum dev_param_types_e  param_type;
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


/* Following macros are used to create DEV_IOCTL() and DEV_CALLBACK() macros */
#define PP_ARG_N( \
          _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
         _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
         _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
         _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
         _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
         _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
         _61, _62, _63, N, ...) N

#define PP_RSEQ_N()                                        \
         62, 61, 60,                                       \
         59, 58, 57, 56, 55, 54, 53, 52, 51, 50,           \
         49, 48, 47, 46, 45, 44, 43, 42, 41, 40,           \
         39, 38, 37, 36, 35, 34, 33, 32, 31, 30,           \
         29, 28, 27, 26, 25, 24, 23, 22, 21, 20,           \
         19, 18, 17, 16, 15, 14, 13, 12, 11, 10,           \
          9,  8,  7,  6,  5,  4,  3,  2,  1,  0 , 0

#define PP_NARG_(...)    PP_ARG_N(__VA_ARGS__)
#define PP_NARG(...)     PP_NARG_(__VA_ARGS__, PP_RSEQ_N())

/* ioctl functions */
uint8_t _ioctl_2_params(struct dev_desc_t * dev,
			uint8_t ioctl_num, void *param1, void *param2);
#define _ioctl_1_params(dev, ioctl_num, param1) \
				_ioctl_2_params(dev, ioctl_num, param1, NULL)
#define _ioctl_0_params(dev, ioctl_num) \
				_ioctl_2_params(dev, ioctl_num, NULL, NULL)
#define _IOCTL_STEP2(a, dev, ...)  _ioctl_##a##_params(dev, __VA_ARGS__)
#define _IOCTL_STEP1(a, dev, ...)  _IOCTL_STEP2(a, dev, __VA_ARGS__)
#define DEV_IOCTL(dev, ...) _IOCTL_STEP1(PP_NARG(__VA_ARGS__), dev, __VA_ARGS__)


/* callback functions */
uint8_t _callback_2_params(struct dev_desc_t * dev,
			uint8_t callback_num, void *param1, void *param2);
#define _callback_1_params(dev, callback_num, param1) \
				_callback_2_params(dev, callback_num, param1, NULL)
#define _callback_0_params(dev, callback_num) \
					_callback_2_params(dev, callback_num, NULL, NULL)
#define _CALLBACK_STEP2(a, dev, ...)  _callback_##a##_params(dev, __VA_ARGS__)
#define _CALLBACK_STEP1(a, dev, ...)  _CALLBACK_STEP2(a, dev, __VA_ARGS__)
#define DEV_CALLBACK(dev, ...)  \
				_CALLBACK_STEP1(PP_NARG(__VA_ARGS__), dev, __VA_ARGS__)

/* end of macros for creating DEV_IOCTL() and DEV_CALLBACK() macros */



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
