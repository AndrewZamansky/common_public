/*
 *
 *   file  :  DevManagment.cpp
 *
 */


#include "_project_typedefs.h"
#include "_project_defines.h"

#if CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0
	#include "errors_api.h"
#endif

#include "dev_management_api.h"
#include "dev_management.h"

#include "auto_init_api.h"
#include "os_wrapper.h"
#include "string.h"


struct dev_desc_t * find_static_dev_by_name(const char *device_name);
void init_new_device(struct dev_desc_t * pdev );
struct dev_param_t const *get_config_param(struct dev_desc_t * pdev,
		char *param_name_str );

#if CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0
	struct dev_desc_t  dev_descriptors[CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES];
#endif


uint8_t _ioctl_2_params(
		struct dev_desc_t * adev, uint8_t ioctl_num, void *param1, void *param2)
{
	if ( NULL == adev )
	{
		CRITICAL_ERROR("access NULL device");
	}
/*	dont check for adev->module or adev->p_runtime_data for static devices
 *  because for very small SOC malloc is not used, so runtime_data MUST be
 *  allocated during compilation time, and for dynamic devices adev will
 *  be NULL if module or p_runtime_data will not be allocated.
 *  if ( NULL == adev->p_runtime_data )	{	}
 */
	return (adev->module->ioctl)(adev, ioctl_num, param1, param2);
}


uint8_t	_callback_2_params(struct dev_desc_t *adev,
						uint8_t callback_num, void *param1, void *param2)
{
	if ( NULL == adev )
	{
		CRITICAL_ERROR("access NULL device");
	}
	/*	dont check for adev->module or adev->p_runtime_data for static devices
	 *  because for very small SOC malloc is not used, so runtime_data MUST be
	 *  allocated during compilation time, and for dynamic devices adev will
	 *  be NULL if module or p_runtime_data will not be allocated.
	 *  if ( NULL == adev->p_runtime_data )	{	}
	 */
	return (adev->module->callback)(adev, callback_num, param1, param2);
}

size_t	DEV_PWRITE(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	if ( NULL == adev )
	{
		CRITICAL_ERROR("access NULL device");
	}
	/*	dont check for adev->module or adev->p_runtime_data for static devices
	 *  because for very small SOC malloc is not used, so runtime_data MUST be
	 *  allocated during compilation time, and for dynamic devices adev will
	 *  be NULL if module or p_runtime_data will not be allocated.
	 *  if ( NULL == adev->p_runtime_data )	{	}
	 */

	return (adev->module->pwrite)(adev, apData, aLength, aOffset);
}

size_t	DEV_PWRITE32(struct dev_desc_t *adev,
			const uint8_t *apData, uint32_t aLength, uint32_t aOffset)
{
	if ( NULL == adev )
	{
		CRITICAL_ERROR("access NULL device");
	}
	/*	dont check for adev->module or adev->p_runtime_data for static devices
	 *  because for very small SOC malloc is not used, so runtime_data MUST be
	 *  allocated during compilation time, and for dynamic devices adev will
	 *  be NULL if module or p_runtime_data will not be allocated.
	 *  if ( NULL == adev->p_runtime_data )	{	}
	 */
	return ((dev_pwrite32_func_t)adev->module->pwrite)(
									adev, apData, aLength, aOffset);
}


size_t	DEV_PREAD(struct dev_desc_t *adev,
			uint8_t *apData, size_t aLength, size_t aOffset)
{
	if ( NULL == adev )
	{
		CRITICAL_ERROR("access NULL device");
	}
	/*	dont check for adev->module or adev->p_runtime_data for static devices
	 *  because for very small SOC malloc is not used, so runtime_data MUST be
	 *  allocated during compilation time, and for dynamic devices adev will
	 *  be NULL if module or p_runtime_data will not be allocated.
	 *  if ( NULL == adev->p_runtime_data )	{	}
	 */
	return (adev->module->pread)(adev, apData, aLength, aOffset);
}

size_t	DEV_PREAD32(struct dev_desc_t *adev,
			uint8_t *apData, uint32_t aLength, uint32_t aOffset)
{
	if ( NULL == adev )
	{
		CRITICAL_ERROR("access NULL device");
	}
	/*	dont check for adev->module or adev->p_runtime_data for static devices
	 *  because for very small SOC malloc is not used, so runtime_data MUST be
	 *  allocated during compilation time, and for dynamic devices adev will
	 *  be NULL if module or p_runtime_data will not be allocated.
	 *  if ( NULL == adev->p_runtime_data )	{	}
	 */
	return ((dev_pread32_func_t)adev->module->pread)(
									adev, apData, aLength, aOffset);
}


/*
 * function : DEV_API_dummy_init_func()
 *
 *
 */
size_t DEV_API_dummy_init_func(struct dev_desc_t *aDevDescriptor)
{
	return 0;
}

/*
 * function : DEV_API_dummy_ioctl_func()
 *
 *
 */
uint8_t DEV_API_dummy_ioctl_func(struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	return 1;
}

/*
 * function : DEV_API_dummy_read_func()
 *
 *
 */
size_t DEV_API_dummy_pread_func(struct dev_desc_t *adev,
						uint8_t *apData, size_t aLength, size_t aOffset)
{
	return 0;
}

/*
 * function : DEV_API_dummy_write_func()
 *
 *
 */
size_t DEV_API_dummy_pwrite_func(struct dev_desc_t *adev,
			const	uint8_t *apData, size_t aLength, size_t aOffset)
{
	return 0;
}


/*
 * function : DEV_API_dummy_callback_func()
 *
 *
 */
uint8_t DEV_API_dummy_callback_func(struct dev_desc_t *adev,
		uint8_t aCallback_num, void * aCallback_param1,	void * aCallback_param2)
{
	return 1;
}


#if defined(CONFIG_USE_DEVICE_NAME_STRINGS)

/*
 * function : DEV_OPEN()
 *
 *
 */
struct dev_desc_t * DEV_OPEN(const char *device_name)
{
	struct dev_desc_t * curr_pdev;

#ifdef CONFIG_USE_SPECIFIC_MEMORY_LOCATION_FOR_DEVICES
	curr_pdev = find_static_dev_by_name(device_name);
	if (NULL != curr_pdev) return curr_pdev;
#elif CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0
	curr_pdev =  &dev_descriptors[0];
	while (curr_pdev < &dev_descriptors[CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES])
	{
		if (0 == strcmp(curr_pdev->dev_name, (char*)device_name ) )
		{
			return  curr_pdev;
		}
		curr_pdev++;
	}
#else
	curr_pdev = NULL;
	(void)curr_pdev;// remove unused variable warning
#endif
	return NULL;
}
#endif /* defined(CONFIG_USE_DEVICE_NAME_STRINGS) */




#if CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0

/*
 * function : DEV_API_add_device()
 *
 *
 */
struct dev_desc_t * DEV_API_add_device(const char* module_name_str,
											const char *device_name_str)
{
	uint32_t i;
	struct dev_desc_t *adev;

	if (NULL == device_name_str)
	{
		return NULL;
	}

	for (i = 0; i < CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES; i++)
	{
		dev = &dev_descriptors[i];
		if (0 == dev->name[0])
		{
			char *new_str;
			uint8_t name_str_len;

			name_str_len = strlen(device_name_str);
			new_str = (char*) os_safe_malloc(name_str_len);
			errors_api_check_if_malloc_succeed(new_str);

			strcpy(new_str,  device_name_str);
			dev->dev_name =  new_str;

			dev->p_config_data	= NULL;
			dev->p_runtime_data	= NULL;
			init_new_device(dev);
			return dev;
		}
	}

	return NULL;
}

#endif


#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION_BY_PARAMETER_NAMES

/*
 * function : DEV_SET_PARAM()
 *
 *
 */
uint8_t DEV_SET_PARAM(char *dev_name_str,
		char *param_name_str, char *param_val_str)
{
	struct dev_param_t  const *config_param;
	struct dev_desc_t * pdev;

	pdev = DEV_OPEN(dev_name_str);
	config_param = get_config_param(pdev, param_name_str);

	if(NULL == config_param) return 1;

	switch (config_param->param_type)
	{
	case DEV_PARAM_TYPE_PDEVICE:
		{
			struct dev_desc_t * param_pdev;
			param_pdev = DEV_OPEN(param_val_str);
			DEV_IOCTL(pdev,
					config_param->paramSetIoctl, param_pdev);
		}
		break;
	case DEV_PARAM_TYPE_UINT8:
		{
			uint8_t param_int;
			param_int = atoi((char*)param_val_str);
			DEV_IOCTL(pdev, config_param->paramSetIoctl, &param_int);
		}
		break;
	case DEV_PARAM_TYPE_UINT32:
		{
			uint32_t param_int;
			param_int = atoi((char*)param_val_str);
			DEV_IOCTL(pdev, config_param->paramSetIoctl, &param_int);
		}
		break;
	case DEV_PARAM_TYPE_MAPPED_SET_TO_SIZE:
		{
			struct mapped_set_to_size_param_t	*p_mapped_set_type_param;
			uint8_t					 mapped_set_size;

			p_mapped_set_type_param = config_param->p_mapped_set_type_param;
			mapped_set_size = config_param->mapped_set_size;
			while(mapped_set_size--)
			{
				if(0 == strcmp(param_val_str, p_mapped_set_type_param->nameStr))
				{
					DEV_IOCTL(pdev, config_param->paramSetIoctl,
												&p_mapped_set_type_param->val);
					break;
				}
				p_mapped_set_type_param++;
			}
		}
		break;
	default :
		return 1;
	}

	return 0;
}


/*
 * function : DEV_SET_PARAM()
 *
 *
 */
uint8_t DEV_GET_PARAM(char *dev_name_str, char *param_name_str,
					char *param_val_str, uint8_t param_val_str_size)
{
	struct dev_param_t  const *config_param;
	struct dev_desc_t * pdev;

	pdev = DEV_OPEN(dev_name_str);
	config_param = get_config_param(pdev, param_name_str );

	if(NULL == config_param) return 1;

	switch (config_param->param_type)
	{
	case DEV_PARAM_TYPE_PDEVICE:
		{
			struct dev_desc_t * param_pdev;
			DEV_IOCTL(pdev, config_param->paramGetIoctl, &param_pdev);
			strncpy(param_val_str, param_pdev->name, param_val_str_size);
		}
		break;
	case DEV_PARAM_TYPE_UINT8:
		{
			uint8_t param_int;
			DEV_IOCTL(pdev, config_param->paramGetIoctl, &param_int);
			//TODO
		}
		break;
	case DEV_PARAM_TYPE_UINT32:
		{
			uint32_t param_int;
			DEV_IOCTL(pdev, config_param->paramGetIoctl, &param_int);
			//TODO
		}
		break;
	case DEV_PARAM_TYPE_MAPPED_SET_TO_SIZE:
		{
			struct mapped_set_to_size_param_t	*p_mapped_set_type_param;
			uint8_t					 mapped_set_size;
			size_t 	val;

			p_mapped_set_type_param = config_param->p_mapped_set_type_param;
			mapped_set_size = config_param->mapped_set_size;
			DEV_IOCTL(pdev, config_param->paramGetIoctl, &val);

			while (mapped_set_size--)
			{
				if (val == p_mapped_set_type_param->val)
				{
					strncpy(param_val_str, p_mapped_set_type_param->nameStr,
							param_val_str_size);
					break;
				}
				p_mapped_set_type_param++;
			}
		}
		break;
	default :
		return 1;
	}

	return 0;
}
#endif
