
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
	( (defined(CONFIG_CORTEX_M3) || defined(CONFIG_CORTEX_M4) || \
			defined(CONFIG_CORTEX_A9) )  && defined(CONFIG_GCC) )

	#define MODULES_PLACEMENT        __attribute__((section("modules_section")))
	#define DEVICE_PLACEMENT     __attribute__((section("static_devs_section")))
	#define DEVICE_DATA_PLACEMENT  __attribute__((section(".static_devs_data")))

#else
	#define MODULES_PLACEMENT

	#define DEVICE_PLACEMENT

	#define DEVICE_DATA_PLACEMENT
#endif

/**********  define API  types ************/


// Macro for adding quotes
#define STRINGIFY(X) STRINGIFY2(X)
#define STRINGIFY2(X) #X

#define STATIC_DEVICE_INCLUDE_NAME(module_name)  \
								STATIC_DEVICE_INCLUDE_NAME2(module_name)

#define STATIC_DEVICE_INCLUDE_NAME2(module_name)  module_name##_add_component.h

#define ADD_CURRENT_DEV  \
						STRINGIFY(STATIC_DEVICE_INCLUDE_NAME(DT_DEV_MODULE))



#define EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(pdev) \
								EXTERN_DECLARATION_TO_STATIC_DEVICE_INST2(pdev)

#define EXTERN_DECLARATION_TO_STATIC_DEVICE_INST2(pdev) \
						extern DEVICE_PLACEMENT struct dev_desc_t inst_##pdev

#define STATIC_DEVICE_INST(pdev)        STATIC_DEVICE_INST2(pdev)
#define STATIC_DEVICE_INST2(pdev)       inst_##pdev
#define P_TO_STATIC_DEVICE_INST(pdev)   &STATIC_DEVICE_INST(pdev)

struct dev_desc_t;

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


typedef enum
{
	PARAMS_STATUS_IDLE=0,
	PARAMS_STATUS_CHANGED_AFTER_LAST_TEST,
	PARAMS_STATUS_NO_CHANGED_AFTER_LAST_TEST
}params_status_t;



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


struct dev_desc_t
{
	void*               p_config_data;
	void*               p_runtime_data;
#if (defined(CONFIG_DYNAMIC_DEVICE_TREE) ||  \
		defined(CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES) || \
		(CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES>0))
	char                *module_name;
#endif
#if defined(CONFIG_USE_DEVICE_NAME_STRINGS)
	char                 *name;
#endif
	dev_ioctl_func_t     ioctl;
	dev_pwrite_func_t    pwrite;
	dev_pread_func_t     pread;
	dev_callback_func_t  callback;
#ifdef CONFIG_USE_SPECIFIC_MEMORY_LOCATION_FOR_DEVICES
	void                 *magic_number;
#endif
};



struct included_module_t
{
	char                 *module_name;
	dev_ioctl_func_t     ioctl;
	dev_pwrite_func_t    pwrite;
	dev_pread_func_t     pread;
	dev_callback_func_t  callback;
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES
	struct dev_param_t   const *config_params_arr;
	uint8_t              size_of_config_params_arr;
#endif
	uint8_t              module_config_struct_size;
	uint8_t              module_runtime_struct_size;
	void                 *magic_number;
};

/**********  define API  functions  ************/

/*  get data functions */
#define DEV_GET_CONFIG_DATA_POINTER(dev)   \
						(((struct dev_desc_t *)dev)->p_config_data)
#define DEV_GET_RUNTIME_DATA_POINTER(dev)  \
						(((struct dev_desc_t *)dev)->p_runtime_data)

/*  ioctl functions */
#define DEV_IOCTL        DEV_IOCTL_1_PARAMS
uint8_t DEV_IOCTL_0_PARAMS(struct dev_desc_t * dev, uint8_t ioctl_num);
uint8_t DEV_IOCTL_1_PARAMS(struct dev_desc_t * dev,
						uint8_t ioctl_num, void *param1);
#define DEV_IOCTL_2_PARAMS(dev, ioctl_num, ioctl_param1, ioctl_param2)    \
		(dev)->ioctl(dev ,ioctl_num,(void*)ioctl_param1,(void*)ioctl_param2)


/* callback functions */
uint8_t DEV_CALLBACK_0_PARAMS(struct dev_desc_t * dev, uint8_t ioctl_num);
uint8_t DEV_CALLBACK_1_PARAMS(struct dev_desc_t * dev,
						uint8_t ioctl_num, void *param1);
#define DEV_CALLBACK_2_PARAMS(dev, callback_num, param1, param2)    \
			dev->callback(dev, callback_num,(void*)param1, (void*)param2)


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
