/*
 *
 * file :   async_rx_wrapper.c
 *
 */

/********  includes *********************/

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "errors_api.h"

#include "dev_management_api.h"

#include "async_rx_wrapper_api.h"
#include "async_rx_wrapper.h"
#include "os_wrapper.h"
#include <string.h>

/**
 * async_rx_wrapper_callback()
 *
 * return:
 */
static uint8_t async_rx_wrapper_callback(struct dev_desc_t *adev ,
	  uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2)
{
	struct async_rx_wrapper_cfg_t *config_handle;
	struct async_rx_wrapper_runtime_t *runtime_handle;
	uint8_t *rx_buff;
	uint8_t *rcvdData ;
	rx_int_size_t rcvdDataLen;
	struct dev_desc_t *   client_dev ;

	rcvdData = (uint8_t *)aCallback_param1;
	rcvdDataLen = (rx_int_size_t)((size_t)aCallback_param2);

	config_handle = DEV_GET_CONFIG_DATA_POINTER(async_rx_wrapper, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(async_rx_wrapper, adev);

	client_dev = config_handle->client_dev;
	if (CALLBACK_DATA_RECEIVED == aCallback_num)
	{
		rx_int_size_t WritePos;
		rx_int_size_t copy_len;
		rx_int_size_t rx_buff_size;

		rx_buff=config_handle->rx_buff;
		if (NULL == rx_buff) return 1;

		WritePos = runtime_handle->WritePos;
		rx_buff_size = config_handle->rx_buff_size;
		if (0 == runtime_handle->isDataInUse)
		{
			rx_int_size_t ReadPos;
			ReadPos = runtime_handle->ReadPos;
			if(WritePos >= rx_buff_size)
			{
				WritePos=0;
				runtime_handle->bufferWasOverflowed = 1;
			}
			else if ( ReadPos > 0)
			{
				WritePos = WritePos - ReadPos;
				memmove(rx_buff, &rx_buff[ReadPos], WritePos);
			}
			runtime_handle->ReadPos=0;
		}

		copy_len=rx_buff_size-WritePos;
		if (copy_len > rcvdDataLen)
		{
			copy_len=rcvdDataLen;
		}

		memcpy(&rx_buff[WritePos], rcvdData, copy_len);
		WritePos=WritePos+copy_len;
		runtime_handle->WritePos=WritePos;

		if(NULL != client_dev)
		{
			return DEV_CALLBACK_0_PARAMS(client_dev, CALLBACK_NEW_DATA_ARRIVED);
		}
	}
	return 0;
}



/**
 * async_rx_wrapper_ioctl()
 *
 * return:
 */
static uint8_t async_rx_wrapper_ioctl(struct dev_desc_t *adev,
		uint8_t aIoctl_num, void * aIoctl_param1 , void * aIoctl_param2)
{
	struct async_rx_wrapper_cfg_t *config_handle;
	struct async_rx_wrapper_runtime_t *runtime_handle;
	rx_int_size_t WritePos,ReadPos;
	struct dev_desc_t *   server_dev;
	struct ioctl_get_data_buffer_t * ioctl_get_data_buffer;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(async_rx_wrapper, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(async_rx_wrapper, adev);

	server_dev  = config_handle->server_dev;

	switch(aIoctl_num)
	{
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION
	case IOCTL_SET_SERVER_DEVICE :
		server_dev = (struct dev_desc_t *)aIoctl_param1;
		config_handle->server_dev = server_dev;
		if (NULL != server_dev)
		{
			DEV_IOCTL(server_dev, IOCTL_SET_ISR_CALLBACK_DEV, adev);
		}
		break;
#ifdef CONFIG_ASYNC_RX_WRAPPER_USE_MALLOC
	case IOCTL_ASYNC_RX_WRAPPER_SET_BUFF_SIZE :
		config_handle->rx_buff_size = *(uint32_t*)aIoctl_param1;
		break;
#endif			//for CONFIG_ASYNC_RX_WRAPPER_ENABLE_RX
#endif // for CONFIG_ASYNC_RX_WRAPPER_USE_RUNTIME_CONFIGURATION
	case IOCTL_SET_ISR_CALLBACK_DEV ://can be used during runtime also
		config_handle->client_dev = (struct dev_desc_t *)aIoctl_param1;
		break;

	case IOCTL_GET_AND_LOCK_DATA_BUFFER :
		runtime_handle->isDataInUse=1; // should be modified first

		// following should be modified after isDataInUse=1 is set
		WritePos=runtime_handle->WritePos;
		ReadPos=runtime_handle->ReadPos;

		ioctl_get_data_buffer = (struct ioctl_get_data_buffer_t *)aIoctl_param1;
		ioctl_get_data_buffer->bufferWasOverflowed =
						runtime_handle->bufferWasOverflowed ;
		runtime_handle->bufferWasOverflowed = 0;
		ioctl_get_data_buffer->TotalLength = WritePos - ReadPos;

		ioctl_get_data_buffer->pBufferStart = &config_handle->rx_buff[ReadPos];
		break;

	case IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER :
		WritePos = runtime_handle->WritePos;
		ReadPos = runtime_handle->ReadPos;
		ReadPos += (size_t)aIoctl_param1;
		if(  ReadPos >  WritePos)
		{
			// should not be reached :
			ReadPos =  WritePos;
		}
		runtime_handle->ReadPos = ReadPos;
		break;

	case IOCTL_SET_UNLOCK_DATA_BUFFER :
		runtime_handle->isDataInUse= 0 ; // should be modified last
		break;

	case IOCTL_DEVICE_START :

#ifdef CONFIG_ASYNC_RX_WRAPPER_USE_MALLOC
		config_handle->rx_buff =
				(uint8_t*)os_safe_malloc(config_handle->rx_buff_size);
		errors_api_check_if_malloc_succeed(config_handle->rx_buff);
#endif

		DEV_IOCTL_0_PARAMS(server_dev, IOCTL_DEVICE_START );
		break;

	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME                       async_rx_wrapper
#define MODULE_IOCTL_FUNCTION             async_rx_wrapper_ioctl
#define MODULE_CALLBACK_FUNCTION          async_rx_wrapper_callback

#define MODULE_CONFIGURABLE_PARAMS_ARRAY  { \
			{	ASYNC_RX_WRAPPER_API_RX_BUFF_SIZE_STR ,                \
				IOCTL_ASYNC_RX_WRAPPER_SET_BUFF_SIZE	, IOCTL_VOID , \
				DEV_PARAM_TYPE_UINT32  , MAPPED_SET_DUMMY_PARAM()  },  \
															\
			{	ASYNC_RX_WRAPPER_API_SERVER_DEVICE_STR ,                \
				IOCTL_SET_SERVER_DEVICE 	, IOCTL_VOID ,              \
				DEV_PARAM_TYPE_PDEVICE , MAPPED_SET_DUMMY_PARAM() },    \
		}

#include "add_module.h"
