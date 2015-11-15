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

#include "sw_uart_wrapper_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "uart_api.h"
#include "_sw_uart_wrapper_prerequirements_check.h" // should be after {sw_uart_wrapper_config.h,dev_managment_api.h}

#include "sw_uart_wrapper_api.h" //place first to test that header file is self-contained
#include "sw_uart_wrapper.h"


/********  defines *********************/


/********  types  *********************/
typedef struct
{
	tx_int_size_t len;
#ifdef SW_UART_WRAPPER_CONFIG_USE_MALLOC
   uint8_t *pData;
#else
   uint8_t pData[SW_UART_WRAPPER_CONFIG_MAX_TX_BUFFER_SIZE];
#endif
} xMessage_t;


typedef void  (*delay_func_t)(uint32_t mSec);

/********  externals *********************/


/********  local defs *********************/


#define INSTANCE(hndl)	((SW_UART_WRAPPER_Instance_t*)hndl)


/**********   external variables    **************/



/***********   local variables    **************/
#if SW_UART_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

	static SW_UART_WRAPPER_Instance_t SW_UART_WRAPPER_InstanceParams[SW_UART_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES] = { {0} };
	static uint16_t usedInstances =0 ;

	static const dev_param_t SW_UART_WRAPPER_Dev_Params[]=
	{
			{IOCTL_SW_UART_WRAPPER_SET_BUFF_SIZE , IOCTL_VOID , (uint8_t*)SW_UART_WRAPPER_API_RX_BUFF_SIZE_STR, NOT_FOR_SAVE},
			{IOCTL_SET_SERVER_DEVICE_BY_NAME , IOCTL_VOID , (uint8_t*)SW_UART_WRAPPER_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},
	};

#endif // for SW_UART_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

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
inline uint8_t SW_UART_WRAPPER_TX_Done(SW_UART_WRAPPER_Instance_t *pInstance,tx_int_size_t transmitedSize)
{
	tx_int_size_t data_length = (tx_int_size_t)pInstance->data_length;
	pdev_descriptor_const   server_dev = pInstance->server_dev;
	pInstance->sendData += transmitedSize;
	if(data_length > transmitedSize)
    {
		data_length -= transmitedSize;
	    DEV_WRITE(server_dev, pInstance->sendData, data_length );
	    pInstance->data_length = data_length;
    }
    else
    {
    	DEV_IOCTL_0_PARAMS(server_dev,IOCTL_UART_DISABLE_TX);

    	pInstance->sendDoneFlag=1;

    }
	return 0;
}


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
inline uint8_t SW_UART_WRAPPER_Data_Received(SW_UART_WRAPPER_Instance_t *pInstance,uint8_t *rcvdData,rx_int_size_t rcvdDataLen)
{
#if  SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE>0
    uint8_t *rx_buff;
    rx_int_size_t WritePos,ReadPos;
    rx_int_size_t copy_len;
    rx_int_size_t rx_buff_size;
    pdev_descriptor_const   client_dev = pInstance->client_dev;

	rx_buff=pInstance->rx_buff;

	if(NULL == rx_buff) return 1;

	WritePos=pInstance->WritePos;
	ReadPos=pInstance->ReadPos;
	rx_buff_size=pInstance->rx_buff_size;

	if( 0 == pInstance->isDataInUse)
	{
		if(WritePos >= rx_buff_size)
		{
			WritePos=0;
			pInstance->bufferWasOverflowed = 1;
		}
		else if ( ReadPos>0)
		{
			WritePos = WritePos - ReadPos;
			memmove((uint8_t*)rx_buff,(uint8_t*)&rx_buff[ReadPos],WritePos);
		}
		pInstance->ReadPos=0;
	}

	copy_len=rx_buff_size-WritePos;
	if(copy_len > rcvdDataLen)
	{
		copy_len=rcvdDataLen;
	}


	memcpy((uint8_t*)&rx_buff[WritePos],rcvdData,copy_len);
	WritePos=WritePos+copy_len;
	pInstance->WritePos=WritePos;

	if(NULL !=client_dev)
	{
		return DEV_CALLBACK_0_PARAMS( client_dev,CALLBACK_NEW_DATA_ARRIVED  ) ;
	}
#endif
	return 1;

}

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
uint8_t sw_uart_wrapper_callback(void * const aHandle ,const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2)
{
	switch(aCallback_num)
	{
		case CALLBACK_TX_DONE :
			return SW_UART_WRAPPER_TX_Done(INSTANCE(aHandle),(tx_int_size_t)((size_t)aCallback_param1));
		   break;
		case CALLBACK_DATA_RECEIVED :
			return SW_UART_WRAPPER_Data_Received(INSTANCE(aHandle),
					(uint8_t *)aCallback_param1,(rx_int_size_t)((size_t)aCallback_param2));
			break;
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
static void sw_uart_send_and_wait_for_end(SW_UART_WRAPPER_Instance_t *pInstance ,
		delay_func_t delay_func,const uint8_t *apData , tx_int_size_t aLength)
{
	tx_int_size_t prev_data_len_left;
	tx_int_size_t curr_data_len_left;
	pdev_descriptor_const   server_dev = pInstance->server_dev;

	pInstance->sendDoneFlag=0;
	pInstance->data_length=aLength;
	pInstance->sendData=apData;

	DEV_IOCTL_0_PARAMS(server_dev ,IOCTL_UART_ENABLE_TX);
	DEV_WRITE(server_dev , apData, aLength);

	prev_data_len_left = aLength + 1;
	while(0 == pInstance->sendDoneFlag)
	{
		delay_func(20);
		curr_data_len_left = pInstance->data_length;
		if(curr_data_len_left < prev_data_len_left) // transmit in progress
		{
			prev_data_len_left = curr_data_len_left;
		}
		else// transmit is stuck
		{
			DEV_IOCTL_0_PARAMS(server_dev ,IOCTL_UART_DISABLE_TX);
			pInstance->data_length=0;
			/*
			* flushing queue . assuming that during 1 msec usb host will access endpoint
			* if there is interrupt from endpoint on the way to CPU, then flush queue
			*/
			delay_func(20);
			break;
		}

	}
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
size_t sw_uart_wrapper_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset)
{
	tx_int_size_t dataLen= (tx_int_size_t)aLength;
	tx_int_size_t curr_transmit_len;

	if(INSTANCE(aHandle)->use_task_for_out)
	{
		xMessage_t xMessage;
		uint8_t *pSendData;
		os_queue_t xQueue = INSTANCE(aHandle)->xQueue;

		if(NULL == xQueue) return 1;


		while(dataLen)
		{
			curr_transmit_len = dataLen;
			if ( SW_UART_WRAPPER_CONFIG_MAX_TX_BUFFER_SIZE < curr_transmit_len )
			{
				curr_transmit_len=SW_UART_WRAPPER_CONFIG_MAX_TX_BUFFER_SIZE;
			}
#ifdef SW_UART_WRAPPER_CONFIG_USE_MALLOC
			xMessage.pData=(uint8_t*)malloc(curr_transmit_len * sizeof(uint8_t));
#endif


			pSendData=xMessage.pData;
			memcpy(pSendData,(uint8_t*)apData,curr_transmit_len);


			xMessage.len=curr_transmit_len;
			if(OS_QUEUE_SEND_SUCCESS != os_queue_send_infinite_wait( xQueue, ( void * ) &xMessage ))
			{
#ifdef SW_UART_WRAPPER_CONFIG_USE_MALLOC
				free(pSendData);
#endif
				return 0;
			}
			dataLen-=curr_transmit_len;
			apData += curr_transmit_len;
		}
	}
	else
	{

		sw_uart_send_and_wait_for_end(INSTANCE(aHandle), busy_delay,apData,dataLen);

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
static void SW_UART_WRAPPER_Send_Task( void *aHandle )
{

	uint8_t *pData;
	xMessage_t xRxMessage;

	os_queue_t xQueue ;

	xQueue = os_create_queue( SW_UART_WRAPPER_CONFIG_MAX_QUEUE_LEN , sizeof(xMessage_t ) );

	INSTANCE(aHandle)->xQueue = xQueue ;

	if( 0 == xQueue  ) return ;

	for( ;; )
	{
		if( OS_QUEUE_RECEIVE_SUCCESS == os_queue_receive_infinite_wait( xQueue , &( xRxMessage )) )
		{
			pData = xRxMessage.pData;
			sw_uart_send_and_wait_for_end(INSTANCE(aHandle), os_delay_ms,pData,xRxMessage.len);

#ifdef SW_UART_WRAPPER_CONFIG_USE_MALLOC
			free( pData );
#endif
		}


#if ((1==INCLUDE_uxTaskGetStackHighWaterMark ) && (1==OS_FREE_RTOS))
		{
			static  size_t stackLeft,minStackLeft=0xffffffff;

			stackLeft = uxTaskGetStackHighWaterMark( NULL );
			if(minStackLeft > stackLeft)
			{
				minStackLeft = stackLeft;
				// !!!! DONT USE PRINTF_DBG . IT CAN CAUSE RECURCIVE LOCK !!
				// COMMENT THIS LINE AS SOON AS POSSIBLE
				PRINTF_DBG("%s   stack left = %d  \r\n" , __FUNCTION__   ,minStackLeft);
			}
		}
#endif
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
uint8_t sw_uart_wrapper_ioctl(void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	rx_int_size_t WritePos,ReadPos;
#if  SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE>0
	WritePos=INSTANCE(aHandle)->WritePos;
	ReadPos=INSTANCE(aHandle)->ReadPos;
#endif

	switch(aIoctl_num)
	{
#if SW_UART_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(const dev_param_t**)aIoctl_param1  = SW_UART_WRAPPER_Dev_Params;
			*(uint8_t*)aIoctl_param2 = sizeof(SW_UART_WRAPPER_Dev_Params)/sizeof(dev_param_t); //size
			break;
		case IOCTL_SET_SERVER_DEVICE_BY_NAME :
			INSTANCE(aHandle)->server_dev = DEV_OPEN((uint8_t*)aIoctl_param1);
			if (NULL!=INSTANCE(aHandle)->server_dev)
			{
				DEV_IOCTL(INSTANCE(aHandle)->server_dev,IOCTL_SET_ISR_CALLBACK_DEV, (void*)INSTANCE(aHandle)->this_dev);
			}
			break;
 #if  SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE>0
		case IOCTL_SW_UART_WRAPPER_SET_BUFF_SIZE :
			INSTANCE(aHandle)->rx_buff_size = atoi((char*)aIoctl_param1);
			INSTANCE(aHandle)->rx_buff = (uint8_t*)malloc(INSTANCE(aHandle)->rx_buff_size);
			break;
		case IOCTL_SET_ISR_CALLBACK_DEV :
			INSTANCE(aHandle)->client_dev = (pdev_descriptor)aIoctl_param1;
			break;
 #endif			//for SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE>0
#endif // for SW_UART_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0

#if  SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE>0
		case IOCTL_GET_AND_LOCK_DATA_BUFFER :
			INSTANCE(aHandle)->isDataInUse=1; // should be modified first

			((ioctl_get_data_buffer_t *)aIoctl_param1)->bufferWasOverflowed = INSTANCE(aHandle)->bufferWasOverflowed ;
			INSTANCE(aHandle)->bufferWasOverflowed = 0;
			((ioctl_get_data_buffer_t *)aIoctl_param1)->TotalLength = WritePos - ReadPos;

			((ioctl_get_data_buffer_t *)aIoctl_param1)->pBufferStart = &INSTANCE(aHandle)->rx_buff[ReadPos];
			break;
		case IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER :
			INSTANCE(aHandle)->ReadPos += (size_t)aIoctl_param1;
			if(  ReadPos >  WritePos)
			{
				// should not be reached :
				INSTANCE(aHandle)->ReadPos =  WritePos;
			}
			break;
		case IOCTL_SET_UNLOCK_DATA_BUFFER :
			INSTANCE(aHandle)->isDataInUse= 0 ; // should be modified last

			break;
#endif // for SW_UART_WRAPPER_CONFIG_MAX_RX_BUFFER_SIZE>0

		case IOCTL_SW_UART_WRAPPER_RESET :
			DEV_IOCTL_0_PARAMS(INSTANCE(aHandle)->server_dev,IOCTL_UART_DISABLE_TX);
			break;

		case IOCTL_SW_UART_WRAPPER_USE_TASK :
			INSTANCE(aHandle)->use_task_for_out = (size_t)aIoctl_param1;
			break;

		case IOCTL_DEVICE_START :
			os_create_task("sw_uart_wrapper_task",SW_UART_WRAPPER_Send_Task,
					aHandle , SW_UART_WRAPPER_CONFIG_TASK_STACK_SIZE , SW_UART_WRAPPER_CONFIG_TASK_PRIORITY);

			break;

		default :
			return 1;
	}
	return 0;
}





#if SW_UART_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SW_UART_WRAPPER_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  sw_uart_wrapper_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{
	SW_UART_WRAPPER_Instance_t *pInstance;
	if(NULL == aDevDescriptor) return 1;
	if (usedInstances >= SW_UART_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES) return 1;

	pInstance = &SW_UART_WRAPPER_InstanceParams[usedInstances ];
	pInstance->this_dev = aDevDescriptor;

	pInstance->use_task_for_out = 1;

	aDevDescriptor->handle = pInstance;
	aDevDescriptor->ioctl = sw_uart_wrapper_ioctl;
	aDevDescriptor->pwrite = sw_uart_wrapper_pwrite;
	aDevDescriptor->callback = sw_uart_wrapper_callback;
	usedInstances++;

	return 0 ;

}
#endif  // for SW_UART_WRAPPER_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0


