/*
 *
 *   file  :  DevManagment.cpp
 *
 */



/***************   includes    *******************/


#include "_dev_managment_prerequirements_check.h" // should be after dev_managment_config.h
#include "dev_managment_api.h" //place first to test that header file is self-contained



/***************   defines    *******************/

/***************   typedefs    *******************/



/**********   external variables    **************/

extern const pdev_descriptor  static_dev_descriptors[];

/***********   global variables    **************/
const dev_descriptor_t dummy_dev=
		{
			"dummy",
			NULL,
			DEV_API_dummy_ioctl_func,
			DEV_API_dummy_pwrite_func,
			DEV_API_dummy_pread_func,
			DEV_API_dummy_callback_func
		};

/***********   local variables    **************/
#if CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0
	dev_descriptor_t  dev_descriptors[CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES];
#endif

/*
 * function : DEV_API_dummy_ioctl_func()
 *
 *
 */
uint8_t DEV_API_dummy_ioctl_func(void * const aHandle ,const uint8_t aIoctl_num ,
		void * aIoctl_param1 , void * aIoctl_param2)
{
	return 1;
}

/*
 * function : DEV_API_dummy_read_func()
 *
 *
 */
size_t DEV_API_dummy_pread_func(const void * const aHandle , uint8_t *apData , size_t aLength, size_t aOffset)
{
	return 0;
}

/*
 * function : DEV_API_dummy_write_func()
 *
 *
 */
size_t DEV_API_dummy_pwrite_func(const void * const aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset)
{
	return 0;
}


/*
 * function : DEV_API_dummy_callback_func()
 *
 *
 */
uint8_t DEV_API_dummy_callback_func(void * const aHandle ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2)
{
	return 1;
}

/*
 * function : DEV_API_open_device()
 *
 *
 */
pdev_descriptor DEV_API_open_device(const uint8_t *device_name)
{
#if (CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 256)
	uint32_t i;
#else
	uint8_t i;
#endif

	pdev_descriptor curr_pdev ;

	i=0;
	while(1)
	{
		 curr_pdev = static_dev_descriptors[i++];
		 if(NULL == curr_pdev) break;
		//name_len=strlen((char*)static_dev_descriptors[i].name);
		if (0 == strcmp( (char*)curr_pdev->name ,(char*) device_name ) )
		{
			return  curr_pdev;
		}
	}

#if CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0
	for(i=0 ; i<CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES ; i++)
	{
		if (0 == strcmp( (char*)dev_descriptors[i].name , (char*)device_name ) )
		{
			return  &dev_descriptors[i];
		}
	}
#endif
	return NULL;
}

#if CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0
/*
 * function : DEV_API_add_device()
 *
 *
 */
pdev_descriptor DEV_API_add_device(const uint8_t *device_name_str,init_dev_descriptor_func_t aInitDescFunc)
{
	uint32_t i;
	pdev_descriptor dev;

	for(i=0 ; i<CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES ; i++)
	{
		dev=&dev_descriptors[i];
		if(0 == dev->name[0])
		{
			if  (NULL != device_name_str)
			{
				if((CONFIG_MAX_DEV_NAME_LEN+1) <= strnlen((char*)device_name_str,(CONFIG_MAX_DEV_NAME_LEN+1)))
				{
					return NULL;
				}
				strcpy((char*)dev->name , (char*)device_name_str );
			}
			else
			{
				snprintf((char*)dev->name, (CONFIG_MAX_DEV_NAME_LEN+1) , "devX%u",(unsigned int)i);
			}
			dev->handle=NULL;
			dev->ioctl = DEV_API_dummy_ioctl_func;
			dev->pread = DEV_API_dummy_pread_func;
			dev->pwrite = DEV_API_dummy_pwrite_func;
			dev->callback = DEV_API_dummy_callback_func;
			aInitDescFunc(dev);
			return dev;
		}
	}


	return NULL;
}
#endif


#ifdef CONFIG_USE_SHELL
/*
 * function : get_dev_ioctl()
 *
 *
 */
uint8_t get_dev_ioctl(uint8_t *dev_name_str, pdev_descriptor *dev_descriptor,
		uint8_t *dev_param_str, const dev_param_t ** pIoctlParam)
{
	const dev_param_t *dev_Params;
	uint8_t dev_Num_Of_Params;

	*dev_descriptor = DEV_OPEN(dev_name_str);
	if(NULL != *dev_descriptor)
	{
		DEV_IOCTL_2_PARAMS(*dev_descriptor , IOCTL_GET_PARAMS_ARRAY_FUNC , &dev_Params, &dev_Num_Of_Params );
		while( dev_Num_Of_Params )
		{
			if(0==memcmp(dev_param_str, dev_Params->paramStr , strlen((char*)dev_Params->paramStr)+1))
			{
				*pIoctlParam = dev_Params;
				return 0;
			}
			dev_Num_Of_Params--;
			dev_Params++;

		}
	}
	else
	{
		return 1;
	}
	return 2;
}
#endif

#if CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0
/*
 * function : DevManagment_API_GetAllDevsArray()
 *
 *
 */
pdev_descriptor DevManagment_API_GetAllDevsArray(void)
{
	return dev_descriptors;
}
#endif

