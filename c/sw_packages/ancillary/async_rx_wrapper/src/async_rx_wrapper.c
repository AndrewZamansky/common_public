/*
 *
 * file :   async_rx_wrapper.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/

#include "_async_rx_wrapper_prerequirements_check.h"

#include "async_rx_wrapper_api.h" //place first to test that header file is self-contained
#include "async_rx_wrapper.h"

#include "async_rx_wrapper_add_component.h"


/********  defines *********************/


/********  types  *********************/


/********  externals *********************/



/**********   external variables    **************/



/***********   local variables    **************/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        async_rx_wrapper_callback                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t async_rx_wrapper_callback(pdev_descriptor_t apdev ,const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2)
{
	async_rx_wrapper_instance_t *config_handle;
	async_rx_wrapper_runtime_instance_t *runtime_handle;
    uint8_t *rx_buff;
    rx_int_size_t WritePos;
    rx_int_size_t copy_len;
    rx_int_size_t rx_buff_size;
    uint8_t *rcvdData ;
    rx_int_size_t rcvdDataLen;
    pdev_descriptor_t   client_dev ;

    rcvdData = (uint8_t *)aCallback_param1;
    rcvdDataLen = (rx_int_size_t)((size_t)aCallback_param2);

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(apdev);

    client_dev = config_handle->client_dev;
    if (CALLBACK_DATA_RECEIVED == aCallback_num)
	{
		rx_buff=config_handle->rx_buff;

		if(NULL == rx_buff) return 1;

		WritePos = runtime_handle->WritePos;
	#ifdef CONFIG_ASYNC_RX_WRAPPER_USE_MALLOC
		rx_buff_size=config_handle->rx_buff_size;
	#else
		rx_buff_size = CONFIG_ASYNC_RX_WRAPPER_RX_BUFFER_SIZE;
	#endif

		if( 0 == runtime_handle->isDataInUse)
		{
			rx_int_size_t ReadPos;
			ReadPos = runtime_handle->ReadPos;
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

		if(NULL != client_dev)
		{
			return DEV_CALLBACK_0_PARAMS( client_dev,CALLBACK_NEW_DATA_ARRIVED  ) ;
		}


	}
	return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        async_rx_wrapper_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t async_rx_wrapper_ioctl(pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2)
{
	async_rx_wrapper_instance_t *config_handle;
	async_rx_wrapper_runtime_instance_t *runtime_handle;
	rx_int_size_t WritePos,ReadPos;
	pdev_descriptor_t   server_dev;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(apdev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(apdev);

	server_dev  = config_handle->server_dev;

	switch(aIoctl_num)
	{
#ifdef CONFIG_USE_RUNTIME_DEVICE_CONFIGURATION
		case IOCTL_SET_SERVER_DEVICE :
			server_dev = (pdev_descriptor_t)aIoctl_param1;
			config_handle->server_dev = server_dev;
			if (NULL != server_dev)
			{
				DEV_IOCTL(server_dev, IOCTL_SET_ISR_CALLBACK_DEV, apdev);
			}
			break;
#ifdef CONFIG_ASYNC_RX_WRAPPER_USE_MALLOC
		case IOCTL_ASYNC_RX_WRAPPER_SET_BUFF_SIZE :
			config_handle->rx_buff_size = *(uint32_t*)aIoctl_param1;
			break;
#endif			//for CONFIG_ASYNC_RX_WRAPPER_ENABLE_RX
#endif // for CONFIG_ASYNC_RX_WRAPPER_USE_RUNTIME_CONFIGURATION
		case IOCTL_SET_ISR_CALLBACK_DEV ://can be used during runtime , not only during configuration
			config_handle->client_dev = (pdev_descriptor_t)aIoctl_param1;
			break;

		case IOCTL_GET_AND_LOCK_DATA_BUFFER :
			runtime_handle->isDataInUse=1; // should be modified first

			WritePos=runtime_handle->WritePos; // should be modified after isDataInUse=1 is set
			ReadPos=runtime_handle->ReadPos; // should be modified after isDataInUse=1 is set

			((ioctl_get_data_buffer_t *)aIoctl_param1)->bufferWasOverflowed = runtime_handle->bufferWasOverflowed ;
			runtime_handle->bufferWasOverflowed = 0;
			((ioctl_get_data_buffer_t *)aIoctl_param1)->TotalLength = WritePos - ReadPos;

			((ioctl_get_data_buffer_t *)aIoctl_param1)->pBufferStart = &config_handle->rx_buff[ReadPos];
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
			config_handle->rx_buff = (uint8_t*)malloc(config_handle->rx_buff_size);
#endif

			DEV_IOCTL_0_PARAMS(server_dev , IOCTL_DEVICE_START );

			break;

		default :
			return 1;
	}
	return 0;
}
