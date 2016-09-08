/*
 *
 * file :   async_tx_wrapper.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_async_tx_wrapper_prerequirements_check.h"

#include "async_tx_wrapper_api.h" //place first to test that header file is self-contained
#include "async_tx_wrapper.h"

#include "async_tx_wrapper_add_component.h"


/********  defines *********************/
#define 	TRANSMIT_IN_PROGRESS	0
#define 	TRANSMIT_DONE			1


/********  types  *********************/
typedef struct
{
	tx_int_size_t len;
#ifdef CONFIG_ASYNC_TX_WRAPPER_USE_MALLOC
   uint8_t *pData;
#else
   uint8_t pData[CONFIG_ASYNC_TX_WRAPPER_MAX_TX_BUFFER_SIZE];
#endif
} xMessage_t;



/********  externals *********************/



/**********   external variables    **************/



/***********   local variables    **************/

static uint8_t dummy_msg;


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        async_tx_wrapper_callback                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t async_tx_wrapper_callback(pdev_descriptor_t apdev ,const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2)
{
	async_tx_wrapper_instance_t *config_handle;
	async_tx_wrapper_runtime_instance_t *runtime_handle;
	tx_int_size_t data_length ;
	pdev_descriptor_t   server_dev ;
	tx_int_size_t	transmitedSize;

	transmitedSize = (tx_int_size_t)((size_t)aCallback_param1);

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(apdev);
	if(CALLBACK_TX_DONE == aCallback_num)
	{
		server_dev = config_handle->server_dev;
		data_length = (tx_int_size_t)runtime_handle->data_length;
		if(data_length > transmitedSize)
	    {
			runtime_handle->sendData += transmitedSize;
			data_length -= transmitedSize;
		    DEV_WRITE(server_dev, runtime_handle->sendData, data_length );
		    runtime_handle->data_length = data_length;
	//	    queueMsg = TRANSMIT_IN_PROGRESS;
	    }
	    else
	    {
	    	os_queue_t xTX_WaitQueue  ;
	    	DEV_IOCTL_0_PARAMS(server_dev,IOCTL_UART_DISABLE_TX);
	    	xTX_WaitQueue = runtime_handle->xTX_WaitQueue;
		    runtime_handle->data_length = 0; // need to be here to avoid additional loop on stuck check
	   		os_queue_send_immediate( xTX_WaitQueue, ( void * ) &dummy_msg);

	//	    queueMsg = TRANSMIT_DONE;
	    }
	}
	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ASYNC_TX_WRAPPER_Send_Task                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void sw_uart_send_and_wait_for_end(pdev_descriptor_t apdev,
			async_tx_wrapper_runtime_instance_t *runtime_handle ,
			xMessage_t *xMessage , uint8_t called_from_task)
{

	async_tx_wrapper_instance_t *config_handle;

	os_queue_t xTX_WaitQueue ;
	pdev_descriptor_t   server_dev ;
	uint8_t *pData ;
	tx_int_size_t length ;
	tx_int_size_t last_checked_length ;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);

	pData = xMessage->pData;
	length = xMessage->len;

	xTX_WaitQueue = runtime_handle->xTX_WaitQueue;
	server_dev = config_handle->server_dev;

	runtime_handle->data_length = length;
	runtime_handle->sendData = pData;

	DEV_IOCTL_0_PARAMS(server_dev ,IOCTL_UART_ENABLE_TX);
	DEV_WRITE(server_dev , pData, length);
	while (length)
	{
		last_checked_length = length;
		if(called_from_task)
		{
			/* when setting timeout = length then for baud rate <8k we should get only message only after
				transmision ends */
			os_queue_receive_with_timeout( xTX_WaitQueue , &( dummy_msg ) , length );
		}
		else
		{
			volatile uint16_t wait;
			wait = 10000 ;
			while (wait--);
		}
		length = runtime_handle->data_length;
		if(length == last_checked_length) /* check if transmition is stucked */
		{
			break ;
		}
	}
	DEV_IOCTL_0_PARAMS(server_dev ,IOCTL_UART_DISABLE_TX);
	runtime_handle->data_length=0;

#ifdef CONFIG_ASYNC_TX_WRAPPER_USE_MALLOC
	free( pData );
#endif

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        async_tx_wrapper_pwrite                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
size_t async_tx_wrapper_pwrite(pdev_descriptor_t apdev ,const uint8_t *apData , size_t aLength, size_t aOffset)
{
	tx_int_size_t dataLen= (tx_int_size_t)aLength;
	tx_int_size_t curr_transmit_len;
	async_tx_wrapper_runtime_instance_t *runtime_handle;
	uint8_t *pSendData;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(apdev);

	while(dataLen)
	{
		xMessage_t xMessage;
		os_queue_t xQueue = runtime_handle->xQueue;

		curr_transmit_len = dataLen;



#ifdef CONFIG_ASYNC_TX_WRAPPER_USE_MALLOC
		xMessage.pData=(uint8_t*)malloc(curr_transmit_len * sizeof(uint8_t));
#else
		if ( CONFIG_ASYNC_TX_WRAPPER_MAX_TX_BUFFER_SIZE < curr_transmit_len )
		{
			curr_transmit_len=CONFIG_ASYNC_TX_WRAPPER_MAX_TX_BUFFER_SIZE;
		}
#endif


		pSendData=xMessage.pData;
		memcpy(pSendData,(uint8_t*)apData,curr_transmit_len);


		xMessage.len=curr_transmit_len;

		if(NULL != xQueue)
		{
			if(OS_QUEUE_SEND_SUCCESS != os_queue_send_infinite_wait( xQueue, ( void * ) &xMessage ))
			{
#ifdef CONFIG_ASYNC_TX_WRAPPER_USE_MALLOC
				free(pSendData);
#endif
				return 0;
			}
		}
		else
		{
			sw_uart_send_and_wait_for_end(apdev , runtime_handle , &xMessage ,0);
		}

		dataLen-=curr_transmit_len;
		apData += curr_transmit_len;
	}


	return aLength;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ASYNC_TX_WRAPPER_Send_Task                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void ASYNC_TX_WRAPPER_Send_Task( void *apdev )
{

	xMessage_t xRxMessage;
	async_tx_wrapper_runtime_instance_t *runtime_handle;

	os_queue_t xQueue ;

	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(apdev);

	xQueue = os_create_queue( CONFIG_ASYNC_TX_WRAPPER_MAX_QUEUE_LEN , sizeof(xMessage_t ) );
	runtime_handle->xQueue = xQueue ;


	if( 0 == xQueue  ) return ;

	for( ;; )
	{
		if( OS_QUEUE_RECEIVE_SUCCESS == os_queue_receive_infinite_wait( xQueue , &xRxMessage ) )
		{
			sw_uart_send_and_wait_for_end(apdev , runtime_handle , &xRxMessage , 1);
		}

		os_stack_test();

	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        async_tx_wrapper_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t async_tx_wrapper_ioctl(pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	async_tx_wrapper_instance_t *config_handle;
	async_tx_wrapper_runtime_instance_t *runtime_handle;
	pdev_descriptor_t   server_dev;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(apdev);

	server_dev  = config_handle->server_dev;

	switch(aIoctl_num)
	{
#ifdef CONFIG_ASYNC_TX_WRAPPER_USE_RUNTIME_CONFIGURATION
		case IOCTL_SET_SERVER_DEVICE_BY_NAME :
			server_dev = DEV_OPEN((uint8_t*)aIoctl_param1);
			config_handle->server_dev = server_dev;
			if (NULL != server_dev)
			{
				DEV_IOCTL(server_dev, IOCTL_SET_ISR_CALLBACK_DEV, (void*)apdev);
			}
			break;

#endif // for CONFIG_ASYNC_TX_WRAPPER_USE_RUNTIME_CONFIGURATION

		case IOCTL_DEVICE_START :
			if(NULL == runtime_handle->xTX_WaitQueue)
			{
				os_queue_t xTX_WaitQueue ;

				xTX_WaitQueue = os_create_queue( 1 , sizeof(uint8_t ) );
				runtime_handle->xTX_WaitQueue = xTX_WaitQueue ;

				os_create_task("async_tx_wrapper_task",ASYNC_TX_WRAPPER_Send_Task,
						apdev , ASYNC_TX_WRAPPER_TASK_STACK_SIZE , ASYNC_TX_WRAPPER_TASK_PRIORITY);
				DEV_IOCTL_0_PARAMS(server_dev , IOCTL_DEVICE_START );
			}
			break;

		default :
			return 1;
	}
	return 0;
}
