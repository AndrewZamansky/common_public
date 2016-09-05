
#ifndef _dev_management_api_h_
#define _dev_management_api_h_


#include "src/_dev_management_prerequirements_check.h"

#define NOT_FOR_SAVE 	0
#define FOR_SAVE 		1


#define END_OF_DEVICE_TREE_STAMP	0xF1


/**********  define API  types ************/

// Macro for adding quotes
#define STRINGIFY(X) STRINGIFY2(X)
#define STRINGIFY2(X) #X

#define STATIC_DEVICE_INCLUDE_NAME(module_name)	STATIC_DEVICE_INCLUDE_NAME2(module_name)
#define STATIC_DEVICE_INCLUDE_NAME2(module_name)	module_name##_add_component.h
#define ADD_CURRENT_DEV  		STRINGIFY(STATIC_DEVICE_INCLUDE_NAME(DT_DEV_MODULE))

#define EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(pdev)	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST2(pdev)
#define EXTERN_DECLARATION_TO_STATIC_DEVICE_INST2(pdev)	extern DEVICE_PLACEMENT dev_descriptor_t inst_##pdev

#define STATIC_DEVICE_INST(pdev)	STATIC_DEVICE_INST2(pdev)
#define STATIC_DEVICE_INST2(pdev)	inst_##pdev
#define P_TO_STATIC_DEVICE_INST(pdev)	&STATIC_DEVICE_INST(pdev)




typedef struct
{
	uint8_t paramSetIoctl;
	uint8_t paramGetIoctl;
	char *paramStr;
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
	CALLBACK_INTERRUPT_ARRIVED ,
	CALLBACK_TX_DONE,
	CALLBACK_DATA_RECEIVED,
	CALLBACK_NEW_DATA_ARRIVED,
	CALLBACK_NEW_DATA_FROM_SOCKET_ARRIVED,
	CALLBACK_BTN_STATE_CHANGED,
	CALLBACK_LAST_COMMON_IOCTL
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

typedef struct _dev_descriptor_t	dev_descriptor_t;
typedef dev_descriptor_t * pdev_descriptor_t;

typedef uint8_t (*dev_ioctl_func_t)(const pdev_descriptor_t apdev ,
		const uint8_t aIoctl_num , void * aIoctl_param1 ,  void * aIoctl_param2)  ;
typedef uint8_t (*dev_ioctl_0_params_func_t)(const pdev_descriptor_t apdev ,const uint8_t aIoctl_num )  ;
typedef uint8_t (*dev_ioctl_1_params_func_t)(const pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void *param1)  ;
typedef size_t (*dev_pwrite_func_t)(const pdev_descriptor_t apdev ,const uint8_t *apData , size_t aLength, size_t aOffset)  ;
typedef uint32_t (*dev_pwrite32_func_t)(const pdev_descriptor_t apdev ,const uint8_t *apData , uint32_t aLength, uint32_t aOffset)  ;
typedef size_t (*dev_write_func_t)(const pdev_descriptor_t apdev ,const uint8_t *apData , size_t aLength)  ;
typedef size_t (*dev_pread_func_t)(const pdev_descriptor_t apdev , uint8_t *apData , size_t aLength, size_t aOffset)  ;
typedef uint32_t (*dev_pread32_func_t)(const pdev_descriptor_t apdev , uint8_t *apData , uint32_t aLength, uint32_t aOffset)  ;
typedef size_t (*dev_read_func_t)(const pdev_descriptor_t apdev , uint8_t *apData , size_t aLength)  ;
typedef uint8_t (*dev_callback_func_t)(const pdev_descriptor_t apdev ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2)  ;
typedef uint8_t (*dev_callback_0_params_func_t)(pdev_descriptor_t apdev , const uint8_t aCallback_num)  ;
typedef uint8_t (*dev_callback_1_params_func_t)(pdev_descriptor_t apdev , const uint8_t aCallback_num, void * aCallback_param1)  ;


struct _dev_descriptor_t
{
	void*    				p_config_data;
	void*    				p_runtime_data;
#if defined(CONFIG_DYNAMIC_DEVICE_TREE) || (CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES>0)
	char	*module_name;
#endif
#if !defined(CONFIG_DONT_USE_DEVICE_NAME_STRINGS)
	char 					*name;
#endif
	dev_ioctl_func_t  		ioctl;
	dev_pwrite_func_t  		pwrite;
	dev_pread_func_t  		pread;
	dev_callback_func_t  	callback;

};

typedef const dev_descriptor_t * pdev_descriptor_const;


typedef struct
{
	char *module_name;
	dev_ioctl_func_t  		ioctl;
	dev_pwrite_func_t  		pwrite;
	dev_pread_func_t  		pread;
	dev_callback_func_t  	callback;
#ifdef CONFIG_USE_SHELL_FOR_DEVICE_CONFIGURATION
	dev_param_t  			*config_params_arr;
#endif
	uint8_t					module_config_struct_size;
	uint8_t					module_runtime_struct_size;
}included_module_t;

/**********  define API  functions  ************/

/*  get data functions */
#define DEV_GET_CONFIG_DATA_POINTER(dev)		(((pdev_descriptor_t)dev)->p_config_data)
#define DEV_GET_RUNTIME_DATA_POINTER(dev)		(((pdev_descriptor_t)dev)->p_runtime_data)

/*  ioctl functions */
uint8_t	DEV_IOCTL_0_PARAMS(pdev_descriptor_t dev , uint8_t ioctl_num);

#define DEV_IOCTL_1_PARAMS(dev,ioctl_num,ioctl_param)   ((dev_ioctl_1_params_func_t)(dev)->ioctl)(dev ,ioctl_num,(void*)ioctl_param)
#define DEV_IOCTL		DEV_IOCTL_1_PARAMS
#define DEV_IOCTL_2_PARAMS(dev,ioctl_num,ioctl_param1,ioctl_param2)    (dev)->ioctl(dev ,ioctl_num,(void*)ioctl_param1,(void*)ioctl_param2)


/* callback functions */
#define DEV_CALLBACK_0_PARAMS(dev,callback_num)    \
		  ((dev_callback_0_params_func_t)(dev)->callback)(dev ,callback_num)
#define DEV_CALLBACK_1_PARAMS(dev,callback_num,callback_param)    \
		  ((dev_callback_1_params_func_t)(dev)->callback)(dev,callback_num,(void*)callback_param)
#define DEV_CALLBACK_2_PARAMS(dev,callback_num,callback_param1,callback_param2)    \
	 	 dev->callback(dev ,callback_num,(void*)callback_param1,(void*)callback_param2)


#define DEV_PWRITE(dev,data,len,offset)    		dev->pwrite(dev ,data,len,offset)
#define DEV_PWRITE32(dev,data,len,offset)    	((dev_pwrite32_func_t)(dev)->pwrite)(dev ,data,len,offset)
uint8_t	DEV_WRITE(const pdev_descriptor_t apdev ,const uint8_t *apData , size_t aLength);
#define DEV_PREAD(dev,data,len,offset)    		dev->pread(dev ,data,len,offset)
#define DEV_PREAD32(dev,data,len,offset)    	((dev_pread32_func_t)(dev)->pread)(dev,data,len,offset)
#define DEV_READ(dev,data,len)    				((dev_read_func_t)(dev)->pread)(dev,data,len)

uint8_t DEV_API_dummy_ioctl_func( pdev_descriptor_t apdev ,
		const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)  ;
uint8_t DEV_API_dummy_callback_func( pdev_descriptor_t apdev ,
		const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2)  ;
size_t DEV_API_dummy_pread_func(const pdev_descriptor_t apdev , uint8_t *apData , size_t aLength, size_t aOffset)  ;
size_t DEV_API_dummy_pwrite_func(const pdev_descriptor_t apdev ,const uint8_t *apData , size_t aLength, size_t aOffset)  ;
size_t DEV_API_dummy_init_func(pdev_descriptor_t aDevDescriptor)  ;
pdev_descriptor_t DEV_OPEN(const char *device_name) ;
uint8_t DEV_SET_PARAM(char *dev_name_str , char *param_name_str , char *param_val_str);



pdev_descriptor_t DevManagment_API_GetAllDevsArray(void);


#endif
