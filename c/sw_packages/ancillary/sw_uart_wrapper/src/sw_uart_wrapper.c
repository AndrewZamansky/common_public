/*
 *
 * file :   sw_uart_wrapper.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_sw_uart_wrapper_prerequirements_check.h"

#include "sw_uart_wrapper_api.h" //place first to test that header file is self-contained
#include "sw_uart_wrapper.h"

#include "sw_uart_wrapper_add_component.h"


/********  defines *********************/
#define 	TRANSMIT_IN_PROGRESS	0
#define 	TRANSMIT_DONE			1


/********  types  *********************/
typedef struct
{
	tx_int_size_t len;
#ifdef CONFIG_SW_UART_WRAPPER_USE_MALLOC
   uint8_t *pData;
#else
   uint8_t pData[CONFIG_SW_UART_WRAPPER_MAX_TX_BUFFER_SIZE];
#endif
} xMessage_t;



/********  externals *********************/



/**********   external variables    **************/



/***********   local variables    **************/

static uint8_t dummy_msg;
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SW_UART_WRAPPER_TX_Done                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
inline uint8_t SW_UART_WRAPPER_TX_Done(sw_uart_wrapper_instance_t *config_handle ,
		sw_uart_wrapper_runtime_instance_t *runtime_handle ,tx_int_size_t transmitedSize)
{
//	uint8_t queueMsg;
	os_queue_t xQueue  ;
	tx_int_size_t data_length ;
	pdev_descriptor_t   server_dev ;

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
    	DEV_IOCTL_0_PARAMS(server_dev,IOCTL_UART_DISABLE_TX);
    	xQueue = runtime_handle->xTX_WaitQueue;
    	if(NULL!=xQueue)
    	{
    		os_queue_send_immediate( xQueue, ( void * ) &dummy_msg);
    	}
//	    queueMsg = TRANSMIT_DONE;
    }

	return 0;
}

#ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SW_UART_WRAPPER_Data_Received                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
inline uint8_t SW_UART_WRAPPER_Data_Received(sw_uart_wrapper_instance_t *config_handle ,
		sw_uart_wrapper_runtime_instance_t *runtime_handle,
		uint8_t *rcvdData , rx_int_size_t rcvdDataLen)
{
    uint8_t *rx_buff;
    rx_int_size_t WritePos,ReadPos;
    rx_int_size_t copy_len;
    rx_int_size_t rx_buff_size;
    pdev_descriptor_t   client_dev = config_handle->client_dev;

	rx_buff=config_handle->rx_buff;

	if(NULL == rx_buff) return 1;

	WritePos = runtime_handle->WritePos;
	ReadPos = runtime_handle->ReadPos;
#ifdef CONFIG_SW_UART_WRAPPER_USE_MALLOC
	rx_buff_size=config_handle->rx_buff_size;
#else
	rx_buff_size = CONFIG_SW_UART_WRAPPER_RX_BUFFER_SIZE;
#endif

	if( 0 == runtime_handle->isDataInUse)
	{
		if(WritePos >= rx_buff_size)
		{
			WritePos=0;
			runtime_handle->bufferWasOverflowed = 1;
		}
		else if ( ReadPos>0)
		{
			WritePos = WritePos - ReadPos;
			memmove((uint8_t*)rx_buff,(uint8_t*)&rx_buff[ReadPos],WritePos);
		}
		runtime_handle->ReadPos=0;
	}

	copy_len=rx_buff_size-WritePos;
	if(copy_len > rcvdDataLen)
	{
		copy_len=rcvdDataLen;
	}


	memcpy((uint8_t*)&rx_buff[WritePos],rcvdData,copy_len);
	WritePos=WritePos+copy_len;
	runtime_handle->WritePos=WritePos;

	if(NULL !=client_dev)
	{
		return DEV_CALLBACK_0_PARAMS( client_dev,CALLBACK_NEW_DATA_ARRIVED  ) ;
	}

	return 1;
}
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        sw_uart_wrapper_callback                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t sw_uart_wrapper_callback(pdev_descriptor_t apdev ,const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2)
{
	sw_uart_wrapper_instance_t *config_handle;
	sw_uart_wrapper_runtime_instance_t *runtime_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(apdev);
	switch(aCallback_num)
	{
		case CALLBACK_TX_DONE :
			return SW_UART_WRAPPER_TX_Done(config_handle , runtime_handle ,(tx_int_size_t)((size_t)aCallback_param1));
		   break;
#ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX
		case CALLBACK_DATA_RECEIVED :

			return SW_UART_WRAPPER_Data_Received(config_handle, runtime_handle,
					(uint8_t *)aCallback_param1,(rx_int_size_t)((size_t)aCallback_param2));
			break;
#endif
		default:
			return 1;
	}
	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SW_UART_WRAPPER_Send_Task                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void sw_uart_send_and_wait_for_end(sw_uart_wrapper_instance_t *config_handle,
			sw_uart_wrapper_runtime_instance_t *runtime_handle ,
		const uint8_t *apData , tx_int_size_t aLength , uint8_t called_from_task)
{

	os_queue_t xTX_WaitQueue ;
	pdev_descriptor_t   server_dev ;

	xTX_WaitQueue = runtime_handle->xTX_WaitQueue;
	server_dev = config_handle->server_dev;

//	uint8_t	xRx_TX_Wait_Message;
	runtime_handle->data_length=aLength;
	runtime_handle->sendData=apData;

	DEV_IOCTL_0_PARAMS(server_dev ,IOCTL_UART_ENABLE_TX);
	DEV_WRITE(server_dev , apData, aLength);
	if(called_from_task)
	{
		os_queue_receive_with_timeout( xTX_WaitQueue , &( dummy_msg ) , aLength);
	}
	else
	{
		busy_delay(aLength);
	}
	DEV_IOCTL_0_PARAMS(server_dev ,IOCTL_UART_DISABLE_TX);
	runtime_handle->data_length=0;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        sw_uart_wrapper_pwrite                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
size_t sw_uart_wrapper_pwrite(pdev_descriptor_t apdev ,const uint8_t *apData , size_t aLength, size_t aOffset)
{
	tx_int_size_t dataLen= (tx_int_size_t)aLength;
	tx_int_size_t curr_transmit_len;
	sw_uart_wrapper_instance_t *config_handle;
	sw_uart_wrapper_runtime_instance_t *runtime_handle;


	uint8_t *pSendData;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(apdev);

	while(dataLen)
	{
		xMessage_t xMessage;
		os_queue_t xQueue = runtime_handle->xQueue;

		curr_transmit_len = dataLen;



#ifdef CONFIG_SW_UART_WRAPPER_USE_MALLOC
		xMessage.pData=(uint8_t*)malloc(curr_transmit_len * sizeof(uint8_t));
#else
		if ( CONFIG_SW_UART_WRAPPER_MAX_TX_BUFFER_SIZE < curr_transmit_len )
		{
			curr_transmit_len=CONFIG_SW_UART_WRAPPER_MAX_TX_BUFFER_SIZE;
		}
#endif


		pSendData=xMessage.pData;
		memcpy(pSendData,(uint8_t*)apData,curr_transmit_len);


		xMessage.len=curr_transmit_len;

		if(config_handle->use_task_for_out)
		{
			if(NULL == xQueue) return 1;

			if(OS_QUEUE_SEND_SUCCESS != os_queue_send_infinite_wait( xQueue, ( void * ) &xMessage ))
			{
#ifdef CONFIG_SW_UART_WRAPPER_USE_MALLOC
				free(pSendData);
#endif
				return 0;
			}
		}
		else
		{
			sw_uart_send_and_wait_for_end(config_handle , runtime_handle ,pSendData,curr_transmit_len,0);
		}

		dataLen-=curr_transmit_len;
		apData += curr_transmit_len;
	}


	return aLength;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SW_UART_WRAPPER_Send_Task                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void SW_UART_WRAPPER_Send_Task( void *apdev )
{

	uint8_t *pData;
	xMessage_t xRxMessage;
	sw_uart_wrapper_instance_t *config_handle;
	sw_uart_wrapper_runtime_instance_t *runtime_handle;

	os_queue_t xQueue ;
	os_queue_t xTX_WaitQueue ;

	xTX_WaitQueue = os_create_queue( 1 , sizeof(uint8_t ) );
	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(apdev);
	runtime_handle->xTX_WaitQueue = xTX_WaitQueue ;

	xQueue = os_create_queue( CONFIG_SW_UART_WRAPPER_MAX_QUEUE_LEN , sizeof(xMessage_t ) );

	runtime_handle->xQueue = xQueue ;

	if( 0 == xQueue  ) return ;

	for( ;; )
	{
		if( OS_QUEUE_RECEIVE_SUCCESS == os_queue_receive_infinite_wait( xQueue , &( xRxMessage )) )
		{
			pData = xRxMessage.pData;
			sw_uart_send_and_wait_for_end(config_handle , runtime_handle ,pData,xRxMessage.len,1);

#ifdef CONFIG_SW_UART_WRAPPER_USE_MALLOC
			free( pData );
#endif
		}

		os_stack_test();

	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        sw_uart_wrapper_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t sw_uart_wrapper_ioctl(pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	sw_uart_wrapper_instance_t *config_handle;
	sw_uart_wrapper_runtime_instance_t *runtime_handle;
	rx_int_size_t WritePos,ReadPos;
	pdev_descriptor_t   server_dev;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(apdev);

#ifdef  CONFIG_SW_UART_WRAPPER_ENABLE_RX
	WritePos=runtime_handle->WritePos;
	ReadPos=runtime_handle->ReadPos;
#endif
	server_dev  = config_handle->server_dev;

	switch(aIoctl_num)
	{
#ifdef CONFIG_SW_UART_WRAPPER_USE_RUNTIME_CONFIGURATION
		case IOCTL_SET_SERVER_DEVICE_BY_NAME :
			server_dev = DEV_OPEN((uint8_t*)aIoctl_param1);
			config_handle->server_dev = server_dev;
			if (NULL != server_dev)
			{
				DEV_IOCTL(server_dev, IOCTL_SET_ISR_CALLBACK_DEV, (void*)apdev);
			}
			break;
#ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX
#ifdef CONFIG_SW_UART_WRAPPER_USE_MALLOC
		case IOCTL_SW_UART_WRAPPER_SET_BUFF_SIZE :
			config_handle->rx_buff_size = atoi((char*)aIoctl_param1);
			break;
#endif			//for CONFIG_SW_UART_WRAPPER_ENABLE_RX
		case IOCTL_SET_ISR_CALLBACK_DEV :
			config_handle->client_dev = (pdev_descriptor_t)aIoctl_param1;
			break;
#endif			//for CONFIG_SW_UART_WRAPPER_ENABLE_RX
#endif // for CONFIG_SW_UART_WRAPPER_USE_RUNTIME_CONFIGURATION

#ifdef CONFIG_SW_UART_WRAPPER_ENABLE_RX
		case IOCTL_GET_AND_LOCK_DATA_BUFFER :
			runtime_handle->isDataInUse=1; // should be modified first

			((ioctl_get_data_buffer_t *)aIoctl_param1)->bufferWasOverflowed = runtime_handle->bufferWasOverflowed ;
			runtime_handle->bufferWasOverflowed = 0;
			((ioctl_get_data_buffer_t *)aIoctl_param1)->TotalLength = WritePos - ReadPos;

			((ioctl_get_data_buffer_t *)aIoctl_param1)->pBufferStart = &config_handle->rx_buff[ReadPos];
			break;
		case IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER :
			runtime_handle->ReadPos += (size_t)aIoctl_param1;
			if(  ReadPos >  WritePos)
			{
				// should not be reached :
				runtime_handle->ReadPos =  WritePos;
			}
			break;
		case IOCTL_SET_UNLOCK_DATA_BUFFER :
			runtime_handle->isDataInUse= 0 ; // should be modified last

			break;
#endif // for CONFIG_SW_UART_WRAPPER_ENABLE_RX

		case IOCTL_SW_UART_WRAPPER_RESET :
			DEV_IOCTL_0_PARAMS(server_dev,IOCTL_UART_DISABLE_TX);
			break;

		case IOCTL_SW_UART_WRAPPER_DONT_USE_TASK :
			config_handle->use_task_for_out = (size_t)aIoctl_param1;
			break;

		case IOCTL_DEVICE_START :
#ifdef CONFIG_SW_UART_WRAPPER_USE_MALLOC
			config_handle->rx_buff = (uint8_t*)malloc(config_handle->rx_buff_size);
#endif
			os_create_task("sw_uart_wrapper_task",SW_UART_WRAPPER_Send_Task,
					apdev , SW_UART_WRAPPER_TASK_STACK_SIZE , SW_UART_WRAPPER_TASK_PRIORITY);
			DEV_IOCTL_0_PARAMS(server_dev , IOCTL_DEVICE_START );
			break;

		default :
			return 1;
	}
	return 0;
}
