/*
 *
 * file :   async_tx_wrapper.c
 *
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_tasks_defines.h"
#include "errors_api.h"

#include "dev_management_api.h"

#include "os_wrapper.h"

#include "async_tx_wrapper_api.h"
#include "async_tx_wrapper.h"
#include "uart_api.h"


#define 	TRANSMIT_IN_PROGRESS	0
#define 	TRANSMIT_DONE			1

struct xMessage_t {
	tx_int_size_t len;
#ifdef CONFIG_ASYNC_TX_WRAPPER_USE_MALLOC
   uint8_t *pData;
#else
   uint8_t pData[CONFIG_ASYNC_TX_WRAPPER_MAX_TX_BUFFER_SIZE];
#endif
};

static uint8_t dummy_msg;

/**
 * async_tx_wrapper_callback()
 *
 * return:
 */
static uint8_t async_tx_wrapper_callback(struct dev_desc_t *adev,
		uint8_t aCallback_num, void * aCallback_param1,
		void * aCallback_param2)
{
	struct async_tx_wrapper_cfg_t *config_handle;
	struct async_tx_wrapper_runtime_t *runtime_handle;
	tx_int_size_t data_length ;
	struct dev_desc_t *   server_dev ;
	tx_int_size_t	transmitedSize;

	transmitedSize = (tx_int_size_t)((size_t)aCallback_param1);

	config_handle = DEV_GET_CONFIG_DATA_POINTER(async_tx_wrapper, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(async_tx_wrapper, adev);

	data_length = (tx_int_size_t)runtime_handle->data_length;

	if (0 == data_length)
	{
		CRITICAL_ERROR("should never be reached\n");
	}

	if ((CALLBACK_TX_DONE == aCallback_num) && (data_length))
	{
		server_dev = config_handle->server_dev;

		if(data_length > transmitedSize)
	    {
			runtime_handle->sendData += transmitedSize;
			data_length -= transmitedSize;
		    DEV_WRITE(server_dev, runtime_handle->sendData, data_length );
	    }
	    else
	    {
	    	os_queue_t xTX_WaitQueue  ;
	    	DEV_IOCTL(server_dev,IOCTL_UART_DISABLE_TX);
	    	xTX_WaitQueue = runtime_handle->xTX_WaitQueue;

	    	/* need to be here to avoid additional loop on stuck check */
	    	data_length = 0;

		    if(NULL != xTX_WaitQueue)
		    {
		        os_queue_send_without_wait(xTX_WaitQueue, ( void * )&dummy_msg);
		    }
	    }
	    runtime_handle->data_length = data_length;
	}

	return 0;
}


/**
 * sw_uart_send_and_wait_for_end()
 *
 * return:
 */
static void sw_uart_send_and_wait_for_end(struct dev_desc_t *adev,
			struct async_tx_wrapper_runtime_t *runtime_handle,
			struct xMessage_t *xMessage, uint8_t called_from_task)
{

	struct async_tx_wrapper_cfg_t *config_handle;
	uint32_t char_timeout;

	os_queue_t xTX_WaitQueue ;
	struct dev_desc_t *   server_dev ;
	uint8_t *pData ;
	tx_int_size_t length ;
	tx_int_size_t last_checked_length ;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(async_tx_wrapper, adev);

	pData = xMessage->pData;
	length = xMessage->len;

	xTX_WaitQueue = runtime_handle->xTX_WaitQueue;
	server_dev = config_handle->server_dev;
	char_timeout = config_handle->char_timeout;

	runtime_handle->sendData = pData;
	runtime_handle->data_length = length;
	DEV_WRITE(server_dev , pData, length);

	while (length)
	{
		last_checked_length = length;
		if(called_from_task)
		{
			/* when setting timeout = length then for baud rate <8k we
			 * should get only message only after transmission ends
			 */
			os_queue_receive_with_timeout(
					xTX_WaitQueue, &dummy_msg, char_timeout );
		}
		else
		{
			volatile uint16_t wait;
			wait = 10000 ;
			while (wait--);
		}
		length = runtime_handle->data_length;
		if (length == last_checked_length)/* check if transmission is stuck */
		{
			break ;
		}
	}

	DEV_IOCTL(server_dev ,IOCTL_UART_DISABLE_TX);
	/* after disabling TX interrupt should never be called
	 * so it's safe to zero data_length
	 */
	runtime_handle->data_length=0;
	if(called_from_task)
	{
		//cleanup queue
		os_queue_receive_with_timeout( xTX_WaitQueue , &( dummy_msg ) , 0 );
	}

#ifdef CONFIG_ASYNC_TX_WRAPPER_USE_MALLOC
	os_safe_free( pData );
#endif

}


/**
 * async_tx_wrapper_pwrite()
 *
 * return:
 */
static size_t async_tx_wrapper_pwrite(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	tx_int_size_t dataLen= (tx_int_size_t)aLength;
	tx_int_size_t curr_transmit_len;
	struct async_tx_wrapper_runtime_t *runtime_handle;
	uint8_t *pSendData;


	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(async_tx_wrapper, adev);

	while(dataLen)
	{
		struct xMessage_t xMessage;
		os_queue_t xQueue = runtime_handle->xQueue;

		curr_transmit_len = dataLen;



#ifdef CONFIG_ASYNC_TX_WRAPPER_USE_MALLOC
		xMessage.pData =
				(uint8_t*)os_safe_malloc(curr_transmit_len * sizeof(uint8_t));
		errors_api_check_if_malloc_succeed(xMessage.pData);

#else
		if ( CONFIG_ASYNC_TX_WRAPPER_MAX_TX_BUFFER_SIZE < curr_transmit_len )
		{
			curr_transmit_len=CONFIG_ASYNC_TX_WRAPPER_MAX_TX_BUFFER_SIZE;
		}
#endif


		pSendData = xMessage.pData;
		if(NULL == pSendData)
		{
			return 0;
		}
		memcpy(pSendData, (uint8_t*)apData, curr_transmit_len);


		xMessage.len=curr_transmit_len;

		if(NULL != xQueue)
		{
			if(OS_QUEUE_SEND_SUCCESS !=
					os_queue_send_infinite_wait(xQueue, ( void * )&xMessage) )
			{
#ifdef CONFIG_ASYNC_TX_WRAPPER_USE_MALLOC
				os_safe_free(pSendData);
#endif
				//PRINT_DATA_DBG("async tx wrapper queue full\n", 27);
				return 0;
			}
		}
		else
		{
			sw_uart_send_and_wait_for_end(adev, runtime_handle, &xMessage ,0);
		}

		dataLen-=curr_transmit_len;
		apData += curr_transmit_len;
	}

	return aLength;

}



/**
 * ASYNC_TX_WRAPPER_Send_Task()
 *
 * return:
 */
static void ASYNC_TX_WRAPPER_Send_Task( void *adev )
{

	struct xMessage_t xRxMessage;
	struct async_tx_wrapper_runtime_t *runtime_handle;

	os_queue_t xQueue ;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(async_tx_wrapper, adev);

	xQueue = os_create_queue(
			CONFIG_ASYNC_TX_WRAPPER_MAX_QUEUE_LEN , sizeof(struct xMessage_t ) );
	runtime_handle->xQueue = xQueue ;


	if( 0 == xQueue  ) return ;

	for( ;; )
	{
		if( OS_QUEUE_RECEIVE_SUCCESS ==
				os_queue_receive_infinite_wait( xQueue , &xRxMessage ) )
		{
			sw_uart_send_and_wait_for_end(adev, runtime_handle, &xRxMessage, 1);
		}

		os_stack_test();
	}

}


/**
 * async_tx_wrapper_ioctl()
 *
 * return:
 */
static uint8_t async_tx_wrapper_ioctl(struct dev_desc_t *adev,
		uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct async_tx_wrapper_cfg_t *config_handle;
	struct async_tx_wrapper_runtime_t *runtime_handle;
	struct dev_desc_t *   server_dev;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(async_tx_wrapper, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(async_tx_wrapper, adev);

	server_dev  = config_handle->server_dev;

	switch(aIoctl_num)
	{
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION
		case IOCTL_SET_SERVER_DEVICE :
			server_dev = (struct dev_desc_t *)aIoctl_param1;
			config_handle->server_dev = server_dev;
			if (NULL != server_dev)
			{
				DEV_IOCTL(server_dev, IOCTL_SET_ISR_CALLBACK_DEV, (void*)adev);
			}
			break;

#endif // for CONFIG_ASYNC_TX_WRAPPER_USE_RUNTIME_CONFIGURATION

		case IOCTL_DEVICE_START :
			if(NULL == runtime_handle->xTX_WaitQueue)
			{
				os_queue_t xTX_WaitQueue ;

				xTX_WaitQueue = os_create_queue( 1 , sizeof(uint8_t ) );
				runtime_handle->xTX_WaitQueue = xTX_WaitQueue ;

				os_create_task("async_tx_wrapper_task",
						ASYNC_TX_WRAPPER_Send_Task, adev,
						ASYNC_TX_WRAPPER_TASK_STACK_SIZE,
						ASYNC_TX_WRAPPER_TASK_PRIORITY);
				DEV_IOCTL(server_dev , IOCTL_DEVICE_START );
			}
			break;

		default :
			return 1;
	}
	return 0;
}

#define MODULE_NAME                         async_tx_wrapper
#define MODULE_IOCTL_FUNCTION               async_tx_wrapper_ioctl
#define MODULE_PWRITE_FUNCTION              async_tx_wrapper_pwrite
#define MODULE_CALLBACK_FUNCTION            async_tx_wrapper_callback

#define MODULE_CONFIGURABLE_PARAMS_ARRAY { \
		{"async_tx_server", IOCTL_SET_SERVER_DEVICE, IOCTL_VOID, \
			DEV_PARAM_TYPE_PDEVICE , MAPPED_SET_DUMMY_PARAM() },\
	}

#include "add_module.h"
