
#ifndef _dev_managment_api_h_
#define _dev_managment_api_h_


#include "src/_dev_managment_prerequirements_check.h"// should be after dev_managment_config.h

#define NOT_FOR_SAVE 	0
#define FOR_SAVE 		1

/**********  define API  types ************/





typedef struct
{
	uint8_t paramSetIoctl;
	uint8_t paramGetIoctl;
	uint8_t *paramStr;
	uint8_t usedForSave;
}dev_param_t;


typedef struct
{
	uint8_t *pBufferStart;
	size_t TotalLength;
	uint8_t bufferWasOverflowed;
}ioctl_get_data_buffer_t;



typedef struct
{
	uint8_t *pData;
	size_t DataLen;
}callback_new_data_from_socket_t;


typedef enum
{
	CALLBACK_VOID ,
	CALLBACK_TX_DONE,
	CALLBACK_DATA_RECEIVED,
	CALLBACK_NEW_DATA_ARRIVED,
	CALLBACK_NEW_DATA_FROM_SOCKET_ARRIVED,
	CALLBACK_BTN_STATE_CHANGED
}common_callback_t;

typedef enum
{
	PARAMS_STATUS_IDLE=0,
	PARAMS_STATUS_CHANGED_AFTER_LAST_TEST,
	PARAMS_STATUS_NO_CHANGED_AFTER_LAST_TEST
}params_status_t;

typedef enum
{
	IOCTL_VOID ,
	IOCTL_GET_PARAMS_ARRAY_FUNC,
	IOCTL_SET_SERVER_DEVICE,
	IOCTL_SET_SERVER_DEVICE_BY_NAME,
	IOCTL_SET_CALLBACK_DEV,
	IOCTL_SET_ISR_CALLBACK_DEV,
	IOCTL_ADD_ISR_CALLBACK_DEV,
	IOCTL_GET_AND_LOCK_DATA_BUFFER,
	IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER,
	IOCTL_SET_UNLOCK_DATA_BUFFER,
	IOCTL_DEVICE_START,
	IOCTL_TEST_PARAMS_STATUS,
	IOCTL_LAST_COMMON_IOCTL
}common_ioctl_t;


typedef uint8_t (*dev_ioctl_func_t)(void * const aHandle ,
		const uint8_t aIoctl_num , void * aIoctl_param1 ,  void * aIoctl_param2)  ;
typedef uint8_t (*dev_ioctl_0_params_func_t)(void * const aHandle ,const uint8_t aIoctl_num )  ;
typedef uint8_t (*dev_ioctl_1_params_func_t)(void * const aHandle ,const uint8_t aIoctl_num , void *param1)  ;
typedef size_t (*dev_pwrite_func_t)(const void * const aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset)  ;
typedef uint32_t (*dev_pwrite32_func_t)(const void * const aHandle ,const uint8_t *apData , uint32_t aLength, uint32_t aOffset)  ;
typedef size_t (*dev_write_func_t)(const void * const aHandle ,const uint8_t *apData , size_t aLength)  ;
typedef size_t (*dev_pread_func_t)(const void *const aHandle , uint8_t *apData , size_t aLength, size_t aOffset)  ;
typedef uint32_t (*dev_pread32_func_t)(const void *const aHandle , uint8_t *apData , uint32_t aLength, uint32_t aOffset)  ;
typedef size_t (*dev_read_func_t)(const void *const aHandle , uint8_t *apData , size_t aLength)  ;
typedef uint8_t (*dev_callback_func_t)(void * const aHandle ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2)  ;
typedef uint8_t (*dev_callback_0_params_func_t)(void * const aHandle , const uint8_t aCallback_num)  ;
typedef uint8_t (*dev_callback_1_params_func_t)(void * const aHandle , const uint8_t aCallback_num, void * aCallback_param1)  ;


typedef struct _dev_descriptor_t
{
	uint8_t 			name[CONFIG_MAX_DEV_NAME_LEN+1];// +1 for null char
	void*    			handle;
	dev_ioctl_func_t  	ioctl;
	dev_pwrite_func_t  	pwrite;
	dev_pread_func_t  	pread;
	dev_callback_func_t  	callback;

}dev_descriptor_t,*pdev_descriptor;

typedef const dev_descriptor_t * pdev_descriptor_const;

typedef uint8_t  (*init_dev_descriptor_func_t)(pdev_descriptor aDevDescriptor);

typedef struct
{
	char *component_name;
	init_dev_descriptor_func_t init_dev_descriptor_func;
}included_module_t;

/**********  define API  functions  ************/

#define DEV_OPEN(dev_name) 						DEV_API_open_device(dev_name)

/*  ioctl functions */
#define DEV_IOCTL_0_PARAMS(dev,ioctl_num)   			((dev_ioctl_0_params_func_t)(dev)->ioctl)((dev)->handle,ioctl_num)
#define DEV_IOCTL_1_PARAMS(dev,ioctl_num,ioctl_param)   ((dev_ioctl_1_params_func_t)(dev)->ioctl)((dev)->handle,ioctl_num,ioctl_param)
#define DEV_IOCTL		DEV_IOCTL_1_PARAMS
#define DEV_IOCTL_2_PARAMS(dev,ioctl_num,ioctl_param1,ioctl_param2)    (dev)->ioctl((dev)->handle,ioctl_num,ioctl_param1,ioctl_param2)

/* callback functions */
#define DEV_CALLBACK_0_PARAMS(dev,callback_num)    \
		  ((dev_callback_0_params_func_t)(dev)->callback)((dev)->handle,callback_num)
#define DEV_CALLBACK_1_PARAMS(dev,callback_num,callback_param)    \
		  ((dev_callback_1_params_func_t)(dev)->callback)((dev)->handle,callback_num,callback_param)
#define DEV_CALLBACK_2_PARAMS(dev,callback_num,callback_param1,callback_param2)    \
	 	 dev->callback((dev)->handle,callback_num,callback_param1,callback_param2)


#define DEV_PWRITE(dev,data,len,offset)    		dev->pwrite((dev)->handle,data,len,offset)
#define DEV_PWRITE32(dev,data,len,offset)    	((dev_pwrite32_func_t)(dev)->pwrite)((dev)->handle,data,len,offset)
#define DEV_WRITE(dev,data,len)    				((dev_write_func_t)(dev)->pwrite)((dev)->handle,data,len)
#define DEV_PREAD(dev,data,len,offset)    		dev->pread((dev)->handle,data,len,offset)
#define DEV_PREAD32(dev,data,len,offset)    	((dev_pread32_func_t)(dev)->pread)((dev)->handle,data,len,offset)
#define DEV_READ(dev,data,len)    				((dev_read_func_t)(dev)->pread)((dev)->handle,data,len)

uint8_t DEV_API_dummy_ioctl_func( void * const aHandle ,
		const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)  ;
uint8_t DEV_API_dummy_callback_func( void * const aHandle ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2)  ;
size_t DEV_API_dummy_pread_func(const void * const aHandle , uint8_t *apData , size_t aLength, size_t aOffset)  ;
size_t DEV_API_dummy_pwrite_func(const void * const aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset)  ;
pdev_descriptor DEV_API_open_device(const uint8_t *device_name) ;
pdev_descriptor DEV_API_add_device(const uint8_t *device_name_str,init_dev_descriptor_func_t aInitDescFunc);

pdev_descriptor DevManagment_API_GetAllDevsArray(void);

extern const dev_descriptor_t dummy_dev;

#define INIT_STATIC_DEVICES(...)  pdev_descriptor_const  static_dev_descriptors[] = {__VA_ARGS__ , NULL}

#define DEV_API_ADD_NEW_STATIC_DEVICE(name,handle,dev_functions)	{name,handle,dev_functions}


#define DEV_API_GET_STATIC_DEVICE(n) (pdev_descriptor)&static_dev_descriptors[n]

#endif
