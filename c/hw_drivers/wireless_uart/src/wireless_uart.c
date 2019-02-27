/*
 *
 * file :   wireless_uart.c
 *
 */



/********  includes *********************/
#include "wireless_uart_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "src/_wireless_uart_prerequirements_check.h" // should be after {wireless_uart_config.h,dev_management_api.h}

#include "wireless_uart_api.h"

#include "wireless_uart.h"

#if WIRELESS_UART_CONFIG_MAX_RX_BUFFER_SIZE <= (1<<8)
	typedef uint8_t wireless_uart_rx_int_size_t;
#else
	#if WIRELESS_UART_CONFIG_MAX_RX_BUFFER_SIZE <= (1<<16)
		typedef uint16_t wireless_uart_rx_int_size_t;
	#else
		typedef uint32_t wireless_uart_rx_int_size_t;
	#endif
#endif


/********  defines *********************/

#define INSTANCE(hndl)	((WIRELESS_UART_Instance_t*)hndl)


/********  types  *********************/
typedef struct
{
	uint8_t dummy;
//	struct dev_desc_t * dev_descriptor;
} xMessage_t;

/********  externals *********************/



/********  local defs *********************/
#if WIRELESS_UART_CONFIG_USE_AS_DYNAMIC_INSTANCE > 0

struct dev_desc_t * this_dev;
static WIRELESS_UART_Instance_t WIRELESS_UART_InstanceParams = { 0 };

static const dev_param_t wireless_uart_Dev_Params[]=
{
		{IOCTL_SET_SERVER_DEVICE_BY_NAME , IOCTL_VOID , (uint8_t*)WIRELESS_UART_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},
};

#endif



uint8_t *currWorkingBuffer;
size_t currWorkingBufferLen;


static os_queue_t xQueue = NULL;




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
uint8_t wireless_uart_callback(struct dev_desc_t *adev ,const uint8_t aCallback_num
		, void * aCallback_param1, void * aCallback_param2)
{
	xMessage_t  queueMsg;
	if (NULL == xQueue)
	{
		return 1;
	}


//	queueMsg.dev_descriptor = (struct dev_desc_t *)aCallback_param1;

	os_queue_send_without_wait( xQueue, ( void * ) &queueMsg);

	return 0;
}

uint8_t send_packet_buff[6];
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        wireless_uart_pwrite                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
size_t wireless_uart_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset)
{
	uint8_t i;
	uint32_t curr_crc;

	curr_crc = wireless_uart_calculate_crc32(apData ,  aLength ) ;

	memcpy(send_packet_buff,"\0***|",6);
	send_packet_buff[5]= 1  + aLength + 4  ; // 1 for size byte , 4 for crc32

	DEV_WRITE(INSTANCE(aHandle)->server_dev , send_packet_buff, 6 );

	DEV_WRITE(INSTANCE(aHandle)->server_dev , apData , aLength );

	for (i=0;i<4;i++)
	{
		send_packet_buff[ 3 - i  ]= curr_crc & 0xff;
		curr_crc = curr_crc >> 8;
	}

	DEV_WRITE(INSTANCE(aHandle)->server_dev , send_packet_buff, 4);


	return aLength;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        wireless_uart_task                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void wireless_uart_task( void *aHandle )
{
	xMessage_t pxRxedMessage;
	wireless_uart_rx_int_size_t curr_buff_pos,total_length;
	wireless_uart_rx_int_size_t i;
	uint8_t *pBufferStart;
	struct ioctl_get_data_buffer_t data_buffer_info;

	uint8_t cnt,packet_size;
	uint32_t received_crc;

	xQueue = os_create_queue( WIRELESS_UART_CONFIG_MAX_QUEUE_LEN , sizeof( xMessage_t ) );

    if( 0 == xQueue  ) return ;

	for( ;; )
	{
		if( OS_QUEUE_RECEIVE_SUCCESS == os_queue_receive_infinite_wait( xQueue, &( pxRxedMessage )) )
		{

			DEV_IOCTL(INSTANCE(aHandle)->server_dev, IOCTL_GET_AND_LOCK_DATA_BUFFER ,  &data_buffer_info);

			total_length = data_buffer_info.TotalLength ;
			pBufferStart = data_buffer_info.pBufferStart ;



			curr_buff_pos =0;
			while (curr_buff_pos < total_length)
			{
				if('*' !=pBufferStart[curr_buff_pos])
				{
					while ( (curr_buff_pos < total_length) && ('*' != pBufferStart[curr_buff_pos]))
					{
						curr_buff_pos++;
					}
				}
				else
				{
					cnt=0;
					while ( (curr_buff_pos < total_length) && ('*' != pBufferStart[curr_buff_pos]))
					{
						cnt++;
						curr_buff_pos++;
					}

					// no more data in buffer , wait for more data to arrive
					// +1 for size byte +1 for START_OF_PACKET byte
					if ( (curr_buff_pos +2) >= total_length) break;

					// if preamble is ok
					if ( ('|' == pBufferStart[curr_buff_pos]) && (2 <= cnt))
					{
						curr_buff_pos++;
						packet_size=pBufferStart[curr_buff_pos];

						// no more data in buffer , wait for more data to arrive
						if ( (curr_buff_pos + packet_size) > total_length) break;


						// check crc
						received_crc=0;
						for(i=0;i<4;i++)
						{
							received_crc += ((uint32_t)pBufferStart[curr_buff_pos + packet_size  - i - 1] << (i*8) );
						}

						if (received_crc == wireless_uart_calculate_crc32(&pBufferStart[curr_buff_pos+1] , packet_size - 4 - 1)) // -1 for size byte , -4 for crc bytes
						{
							currWorkingBuffer = &pBufferStart[curr_buff_pos + 1];
							currWorkingBufferLen = packet_size - 1 - 4 ; // -1 for size byte , -4 for crc bytes
							if( 0 == DEV_CALLBACK_1_PARAMS( INSTANCE(aHandle)->callback_dev,
									CALLBACK_NEW_DATA_ARRIVED ,  INSTANCE(aHandle)->this_dev  ) )
							{
								while(currWorkingBufferLen)
								{
									os_delay_ms(1);
								}
							}
							//  discard all packet
							curr_buff_pos +=packet_size;


						}
						else
						{
							// crc NOT OK . discard all data
							curr_buff_pos=total_length;
						}


					}
					else
					{
						//preamble is NOT ok
						curr_buff_pos=total_length;
					}



				}

				//data_used_in_buffer_info.BytesWasConsumed = curr_buff_pos;
				//DEV_IOCTL(INSTANCE(aHandle)->server_dev, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER , (uint32_t)&data_used_in_buffer_info);
				DEV_IOCTL(INSTANCE(aHandle)->server_dev, IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER ,(void*) (size_t)curr_buff_pos);
				total_length -= curr_buff_pos;
				pBufferStart += curr_buff_pos;
				curr_buff_pos =0;


			}

			DEV_IOCTL_0_PARAMS(INSTANCE(aHandle)->server_dev, IOCTL_SET_UNLOCK_DATA_BUFFER );


		}

		os_stack_test();

	}

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HTTP_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t wireless_uart_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{

	struct dev_desc_t * server_device;

	switch(aIoctl_num)
	{
#if WIRELESS_UART_CONFIG_USE_AS_DYNAMIC_INSTANCE > 0
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(const dev_param_t**)aIoctl_param1  = wireless_uart_Dev_Params;
			*(uint8_t*)aIoctl_param2 =  sizeof(wireless_uart_Dev_Params)/sizeof(dev_param_t); //size
			break;

		case IOCTL_SET_SERVER_DEVICE_BY_NAME :
			server_device = DEV_OPEN((uint8_t*)aIoctl_param1);
			if(NULL != server_device)
			{
				DEV_IOCTL(server_device, IOCTL_SET_ISR_CALLBACK_DEV ,  INSTANCE(aHandle)->this_dev);
			}
			INSTANCE(aHandle)->server_dev = server_device;
			break;
		case IOCTL_SET_ISR_CALLBACK_DEV :
			INSTANCE(aHandle)->callback_dev = (struct dev_desc_t *)aIoctl_param1;
			break;
#endif

		case IOCTL_GET_AND_LOCK_DATA_BUFFER :

			((struct ioctl_get_data_buffer_t *)aIoctl_param1)->TotalLength = currWorkingBufferLen;

			((struct ioctl_get_data_buffer_t *)aIoctl_param1)->pBufferStart = currWorkingBuffer;
			break;
		case IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER :
//			tmp = ((ioctl_set_bytes_consumed_in_data_buffer_t *) aIoctl_param1)->BytesWasConsumed;
			currWorkingBuffer += (size_t)aIoctl_param1	;
			currWorkingBufferLen -= (size_t)aIoctl_param1 ;

			break;
		case IOCTL_SET_UNLOCK_DATA_BUFFER :


			break;
		case IOCTL_DEVICE_START :
			os_create_task("wireless_uart_task",wireless_uart_task,
					(void*) aHandle , WIRELESS_UART_CONFIG_TASK_STACK_SIZE , WIRELESS_UART_CONFIG_TASK_PRIORITY);
			break;

		default :
			return 1;
	}
	return 0;
}

#if WIRELESS_UART_CONFIG_USE_AS_DYNAMIC_INSTANCE > 0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        wireless_uart_init_dev_descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  wireless_uart_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor)
{

	if(NULL == aDevDescriptor) return 1;
	aDevDescriptor->handle = &WIRELESS_UART_InstanceParams;
	WIRELESS_UART_InstanceParams.this_dev = aDevDescriptor;

	aDevDescriptor->ioctl = wireless_uart_ioctl;
	aDevDescriptor->callback = wireless_uart_callback;
	aDevDescriptor->pwrite = wireless_uart_pwrite ;

	return 0;
}

#endif

