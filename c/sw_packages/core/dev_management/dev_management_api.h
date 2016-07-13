
#ifndef _dev_management_api_h_
#define _dev_management_api_h_


#include "src/_dev_management_prerequirements_check.h"// should be after dev_management_config.h

#define NOT_FOR_SAVE 	0
#define FOR_SAVE 		1

#define DEVICE_MAGIC_NUMBER	0x1B3D

// Macro for adding quotes
#define STRINGIFY(X) STRINGIFY2(X)
#define STRINGIFY2(X) #X

//#define INIT_CURRENT_DEV()  STRINGIFY(CURRENT_DEV()_add_static_device.h)
//#define ADD_CURRENT_DEV()  	STRINGIFY(CURRENT_DEV()_add_static_device.h)
#define STATIC_DEVICE_INCLUDE_NAME(a)	STATIC_DEVICE_INCLUDE_NAME2(a)
#define STATIC_DEVICE_INCLUDE_NAME2(a)	a##_add_static_device.h
#define INIT_CURRENT_DEV()  STRINGIFY(STATIC_DEVICE_INCLUDE_NAME(CURRENT_DEV))
#define ADD_CURRENT_DEV()  	STRINGIFY(STATIC_DEVICE_INCLUDE_NAME(CURRENT_DEV))


#define EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(pdev)	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST2(pdev)
#define EXTERN_DECLARATION_TO_STATIC_DEVICE_INST2(pdev)	extern dev_descriptor_t inst_##pdev
#define STATIC_DEVICE_INNER_INST(pdev)	STATIC_DEVICE_INNER_INST2(pdev)
#define STATIC_DEVICE_INNER_INST2(pdev)	inner_inst_##pdev
#define STATIC_DEVICE_INST(pdev)	STATIC_DEVICE_INST2(pdev)
#define STATIC_DEVICE_INST2(pdev)	inst_##pdev
#define P_TO_STATIC_DEVICE_INST(pdev)	&STATIC_DEVICE_INST(pdev)

#define CREATE_STATIC_DEVICE(pdev , dev_handle , dev_ioctl , dev_pwrite , dev_pread , dev_callback)  \
	 CREATE_STATIC_DEVICE2(pdev , dev_handle , dev_ioctl , dev_pwrite , dev_pread , dev_callback)
#define CREATE_STATIC_DEVICE2(pdev , dev_handle , dev_ioctl , dev_pwrite , dev_pread , dev_callback)  \
		DEVICE_PLACEMENT dev_descriptor_t STATIC_DEVICE_INST(pdev) =	\
			{											\
				/*DEVICE_MAGIC_NUMBER,					*/\
				""#pdev,								\
				dev_handle,								\
				dev_ioctl,								\
				dev_pwrite,								\
				dev_pread,								\
				dev_callback							\
			};											\
			pdev_descriptor_t pdev = P_TO_STATIC_DEVICE_INST(pdev)


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
//	uint16_t 				magic_number;
	char 					*name;
	void*    				handle;
	dev_ioctl_func_t  		ioctl;
	dev_pwrite_func_t  		pwrite;
	dev_pread_func_t  		pread;
	dev_callback_func_t  	callback;

}dev_descriptor_t,*pdev_descriptor_t;

typedef const dev_descriptor_t * pdev_descriptor_const;

typedef uint8_t  (*init_dev_descriptor_func_t)(pdev_descriptor_t aDevDescriptor);

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
pdev_descriptor_t DEV_API_open_device(const uint8_t *device_name) ;
pdev_descriptor_t DEV_API_add_device(const uint8_t *device_name_str,init_dev_descriptor_func_t aInitDescFunc);

#if 0
void DEV_API_auto_start_devices(void);
#endif

pdev_descriptor_t DevManagment_API_GetAllDevsArray(void);


#endif
