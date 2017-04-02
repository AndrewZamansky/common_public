/*
 *
 *   file  :  DevManagment.cpp
 *
 */



/***************   includes    *******************/

#include "dev_management_api.h"
#include "_dev_management_prerequirements_check.h"
#include "dev_management.h"

#include "auto_init_api.h"


/***************   defines    *******************/

/***************   typedefs    *******************/



/**********   external variables    **************/
extern uint32_t start_of_device_tree_stamp ;
extern uint32_t start_of_modules_stamp ;
extern uint8_t end_of_modules_stamp ;


/***********   global variables    **************/

/***********   local variables    **************/
#if CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0
	dev_descriptor_t  dev_descriptors[CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES];
#endif

uint8_t	DEV_IOCTL_0_PARAMS(pdev_descriptor_t dev,uint8_t ioctl_num)
{
	return ((dev)->ioctl)(dev, ioctl_num, NULL, NULL);
}

uint8_t	DEV_IOCTL_1_PARAMS(pdev_descriptor_t dev,
						uint8_t ioctl_num, void *param1)
{
	return ((dev)->ioctl)(dev, ioctl_num, param1, NULL);
}


uint8_t	DEV_CALLBACK_0_PARAMS(pdev_descriptor_t dev,uint8_t ioctl_num)
{
	return ((dev)->callback)(dev, ioctl_num, NULL, NULL);
}

uint8_t	DEV_CALLBACK_1_PARAMS(pdev_descriptor_t dev,
						uint8_t ioctl_num, void *param1)
{
	return ((dev)->callback)(dev, ioctl_num, param1, NULL);
}



size_t	DEV_WRITE(const pdev_descriptor_t apdev ,
					const uint8_t *apData, size_t aLength )
{
	return ((dev_write_func_t)(apdev)->pwrite)(apdev, apData, aLength);
}

/*
 * function : DEV_API_dummy_init_func()
 *
 *
 */
size_t DEV_API_dummy_init_func(pdev_descriptor_t aDevDescriptor)
{
	return 0;
}

/*
 * function : DEV_API_dummy_ioctl_func()
 *
 *
 */
uint8_t DEV_API_dummy_ioctl_func(pdev_descriptor_t apdev,
					const uint8_t aIoctl_num, void * aIoctl_param1,
					void * aIoctl_param2)
{
	return 1;
}

/*
 * function : DEV_API_dummy_read_func()
 *
 *
 */
size_t DEV_API_dummy_pread_func(const pdev_descriptor_t apdev,
						uint8_t *apData, size_t aLength, size_t aOffset)
{
	return 0;
}

/*
 * function : DEV_API_dummy_write_func()
 *
 *
 */
size_t DEV_API_dummy_pwrite_func(const pdev_descriptor_t apdev,
				const uint8_t *apData, size_t aLength, size_t aOffset)
{
	return 0;
}


/*
 * function : DEV_API_dummy_callback_func()
 *
 *
 */
uint8_t DEV_API_dummy_callback_func(pdev_descriptor_t apdev,
		const uint8_t aCallback_num, void * aCallback_param1,
		void * aCallback_param2)
{
	return 1;
}


#if !defined(CONFIG_DONT_USE_DEVICE_NAME_STRINGS)

/*
 * function : DEV_OPEN()
 *
 *
 */
pdev_descriptor_t DEV_OPEN(const char *device_name)
{
	pdev_descriptor_t curr_pdev ;

	curr_pdev =
			(pdev_descriptor_t)(((uint8_t*)&start_of_device_tree_stamp) + 4);
	while (*(uint8_t*)curr_pdev != END_OF_DEVICE_TREE_STAMP)
	{
		//name_len=strlen((char*)static_dev_descriptors[i].name);
		if (0 == strcmp( (char*)curr_pdev->name , device_name ) )
		{
			return  curr_pdev;
		}
		curr_pdev++;
	}

#if CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0
	curr_pdev =  &dev_descriptors[0];
	while(curr_pdev <  &dev_descriptors[CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES])
	{
		if (0 == strcmp(curr_pdev->name , (char*)device_name ) )
		{
			return  curr_pdev;
		}
		curr_pdev++;
	}
#endif
	return NULL;
}
#endif /* !defined(CONFIG_DONT_USE_DEVICE_NAME_STRINGS) */

#if defined(CONFIG_DYNAMIC_DEVICE_TREE) || (CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES>0)

/*
 * function : add_new_device()
 *
 *
 */
static void init_new_device(pdev_descriptor_t pdev )
{
	const char *module_name_str ;
	included_module_t *curr_include_module;

	module_name_str = pdev->module_name;
	curr_include_module =
			(included_module_t*)(((uint8_t*)&start_of_modules_stamp) + 4);
	while ((uint8_t*)curr_include_module < &end_of_modules_stamp)
	{
		if (0 == strcmp(module_name_str , curr_include_module->module_name))
		{
			pdev->ioctl		= curr_include_module->ioctl;
			pdev->pwrite	= curr_include_module->pwrite;
			pdev->pread		= curr_include_module->pread;
			pdev->callback	= curr_include_module->callback;
			if (NULL == pdev->p_config_data)
			{
				pdev->p_config_data =
						malloc(curr_include_module->module_config_struct_size);
			}
			pdev->p_runtime_data =
					malloc(curr_include_module->module_runtime_struct_size);
			break;
		}
		curr_include_module++;
	}
	CRITICAL_ERROR("module not found\n");
}

#endif



#if defined(CONFIG_DYNAMIC_DEVICE_TREE)
/*
 * function : init_device_tree()
 *
 *
 */
void init_device_tree()
{
	uint32_t *src ;
	uint32_t *dst ;
	pdev_descriptor_t curr_pdev ;

	src = (uint32_t *)CONFIG_DEVICE_TREE_LOCATION_ADDR;
	dst = &start_of_device_tree_stamp;
	if (src != dst)
	{
		while ((uint8_t*)dst < ( ((uint8_t*)&start_of_device_tree_stamp) +
				CONFIG_DEVICE_TREE_MAXIMAL_SIZE))
		{
		  *dst++ = *src++; /* 4 bytes copy*/
		}
	}

	curr_pdev =
			(pdev_descriptor_t)(((uint8_t*)&start_of_device_tree_stamp) + 4);
	while (*(uint8_t*)curr_pdev != END_OF_DEVICE_TREE_STAMP)
	{
		init_new_device(curr_pdev);
		curr_pdev++;
	}

}

AUTO_INIT_FUNCTION(init_device_tree);

#endif



#if CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0

/*
 * function : DEV_API_add_device()
 *
 *
 */
pdev_descriptor_t DEV_API_add_device(const char* module_name_str,
											const char *device_name_str)
{
	uint32_t i;
	pdev_descriptor_t dev;

	if (NULL == device_name_str)
	{
		return NULL;
	}

	for (i=0 ; i < CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES ; i++)
	{
		dev = &dev_descriptors[i];
		if(0 == dev->name[0])
		{
			char *new_str;
			uint8_t name_str_len;

			name_str_len = strlen(device_name_str);
			new_str = (char*) malloc(name_str_len);
			strcpy(new_str,  module_name_str);
			dev->module_name =  new_str;

			name_str_len = strlen(device_name_str);
			new_str = (char*) malloc(name_str_len);
			strcpy(new_str,  device_name_str);
			dev->name =  new_str;

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
dev_param_t const *get_config_param(pdev_descriptor_t pdev,
		char *param_name_str )
{
	const char *module_name_str ;
	included_module_t *curr_include_module;

	module_name_str = pdev->module_name;
	curr_include_module =
			(included_module_t*)(((uint8_t*)&start_of_modules_stamp) + 4);
	while ((uint8_t*)curr_include_module < &end_of_modules_stamp)
	{
		if (0 == strcmp(module_name_str , curr_include_module->module_name))
		{
			uint8_t		 size_of_config_params_arr;
			dev_param_t  const *config_param;
			config_param = curr_include_module -> config_params_arr;
			size_of_config_params_arr =
					curr_include_module -> size_of_config_params_arr;
			while (size_of_config_params_arr--)
			{
				if (0 == strcmp(param_name_str, config_param->paramNameStr))
				{
					return config_param;
				}
				config_param++;
			}
			return NULL;
		}
		curr_include_module++;
	}
	return NULL;
}

/*
 * function : DEV_SET_PARAM()
 *
 *
 */
uint8_t DEV_SET_PARAM(char *dev_name_str,
		char *param_name_str, char *param_val_str)
{
	dev_param_t  const *config_param;
	pdev_descriptor_t pdev;

	pdev = DEV_OPEN(dev_name_str);
	config_param = get_config_param(pdev, param_name_str);

	if(NULL == config_param) return 1;

	switch(config_param->param_type)
	{
	case DEV_PARAM_TYPE_PDEVICE:
		{
			pdev_descriptor_t param_pdev;
			param_pdev = DEV_OPEN(param_val_str);
			DEV_IOCTL_1_PARAMS(pdev,
					config_param->paramSetIoctl, param_pdev);
		}
		break;
	case DEV_PARAM_TYPE_UINT8:
		{
			uint8_t param_int;
			param_int = atoi((char*)param_val_str);
			DEV_IOCTL_1_PARAMS(pdev, config_param->paramSetIoctl, &param_int);
		}
		break;
	case DEV_PARAM_TYPE_UINT32:
		{
			uint32_t param_int;
			param_int = atoi((char*)param_val_str);
			DEV_IOCTL_1_PARAMS(pdev, config_param->paramSetIoctl, &param_int);
		}
		break;
	case DEV_PARAM_TYPE_MAPPED_SET_TO_SIZE:
		{
			mapped_set_to_size_param_t	*p_mapped_set_type_param;
			uint8_t					 mapped_set_size;

			p_mapped_set_type_param = config_param->p_mapped_set_type_param;
			mapped_set_size = config_param->mapped_set_size;
			while(mapped_set_size--)
			{
				if(0 == strcmp(param_val_str, p_mapped_set_type_param->nameStr))
				{
					DEV_IOCTL_1_PARAMS(pdev, config_param->paramSetIoctl,
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
	dev_param_t  const *config_param;
	pdev_descriptor_t pdev;

	pdev = DEV_OPEN(dev_name_str);
	config_param = get_config_param(pdev, param_name_str );

	if(NULL == config_param) return 1;

	switch (config_param->param_type)
	{
	case DEV_PARAM_TYPE_PDEVICE:
		{
			pdev_descriptor_t param_pdev;
			DEV_IOCTL_1_PARAMS(pdev, config_param->paramGetIoctl, &param_pdev);
			strncpy(param_val_str, param_pdev->name, param_val_str_size);
		}
		break;
	case DEV_PARAM_TYPE_UINT8:
		{
			uint8_t param_int;
			DEV_IOCTL_1_PARAMS(pdev, config_param->paramGetIoctl, &param_int);
			//TODO
		}
		break;
	case DEV_PARAM_TYPE_UINT32:
		{
			uint32_t param_int;
			DEV_IOCTL_1_PARAMS(pdev, config_param->paramGetIoctl, &param_int);
			//TODO
		}
		break;
	case DEV_PARAM_TYPE_MAPPED_SET_TO_SIZE:
		{
			mapped_set_to_size_param_t	*p_mapped_set_type_param;
			uint8_t					 mapped_set_size;
			size_t 	val;

			p_mapped_set_type_param = config_param->p_mapped_set_type_param;
			mapped_set_size = config_param->mapped_set_size;
			DEV_IOCTL_1_PARAMS(pdev, config_param->paramGetIoctl, &val);

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

#if CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0
/*
 * function : DevManagment_API_GetAllDevsArray()
 *
 *
 */
pdev_descriptor_t DevManagment_API_GetAllDevsArray(void)
{
	return dev_descriptors;
}
#endif
