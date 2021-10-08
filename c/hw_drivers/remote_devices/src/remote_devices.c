/*
 *
 *   file  :  REMOTE_DEVICES.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h"

#include "remote_devices_api.h"
#include "semphr.h"
#include "serial_number_api.h"


#define REMOTE_DEVICES_MAX_NUM_OF_GPIOS 2

#define REMOTE_DEVICES_OUT 1


struct struct REMOTE_DEVICES_Instance_t{
	struct dev_desc_t * server_dev;
	struct dev_desc_t * this_dev;
	SemaphoreHandle_t mutex;
	volatile uint8_t  transaction_done;
	uint8_t *currRemoteDeviceAddr;
	uint8_t *pResult;
} ;




extern uint8_t *pSerNum;


static struct REMOTE_DEVICES_Instance_t 
		REMOTE_DEVICES_InstanceParams[REMOTE_DEVICES_MAX_NUM_OF_GPIOS];
static uint16_t usedInstances =0 ;

static const dev_param_t REMOTE_DEVICES_Dev_Params[]=
{
		{IOCTL_SET_SERVER_DEVICE_BY_NAME, IOCTL_VOID,
			(uint8_t*)REMOTE_DEVICES_API_SERVER_DEVICE_STR, NOT_FOR_SAVE},
};

#define INSTANCE(hndl)	((struct REMOTE_DEVICES_Instance_t*)hndl)



uint8_t remote_devices_callback(struct dev_desc_t *adev ,const uint8_t aCallback_num
		, void * aCallback_param1, void * aCallback_param2)
{
	struct ioctl_get_data_buffer_t data_buffer_info;
	uint8_t total_length;
	uint8_t *pBufferStart;

	DEV_IOCTL((struct dev_desc_t *)aCallback_param1,
			IOCTL_GET_AND_LOCK_DATA_BUFFER ,  &data_buffer_info);

	total_length = data_buffer_info.TotalLength ;
	pBufferStart = data_buffer_info.pBufferStart ;

	if(0 == memcmp(pBufferStart , pSerNum , SERIAL_NUMBER_LEN))
	{
		if(0 == memcmp(&pBufferStart[SERIAL_NUMBER_LEN],
				INSTANCE(aHandle)->currRemoteDeviceAddr , SERIAL_NUMBER_LEN))
		{
			memcpy(INSTANCE(aHandle)->pResult,
				&pBufferStart[2*SERIAL_NUMBER_LEN],4);
			INSTANCE(aHandle)->transaction_done = 1;
		}
	}

	DEV_IOCTL((struct dev_desc_t *)aCallback_param1,
		IOCTL_SET_BYTES_CONSUMED_IN_DATA_BUFFER,
		(void*)(uint32_t)total_length);
	DEV_IOCTL(
		(struct dev_desc_t *)aCallback_param1, IOCTL_SET_UNLOCK_DATA_BUFFER );

	return 0;
}

static uint8_t packet_buff[48];


uint8_t remote_devices_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	uint16_t tries,count;


	struct dev_desc_t * server_dev;

	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(const dev_param_t**)aIoctl_param1  = REMOTE_DEVICES_Dev_Params;
			*(uint8_t*)aIoctl_param2 =
				sizeof(REMOTE_DEVICES_Dev_Params)/sizeof(dev_param_t); //size
			break;

		case IOCTL_SET_SERVER_DEVICE_BY_NAME :
			server_dev = DEV_OPEN((uint8_t*)aIoctl_param1);
			INSTANCE(aHandle)->server_dev = server_dev;
			if(NULL != server_dev)
			{
				DEV_IOCTL(server_dev,
					IOCTL_SET_ISR_CALLBACK_DEV ,  INSTANCE(aHandle)->this_dev);
			}			break;


		case IOCTL_DEVICE_START :
			INSTANCE(aHandle)->mutex = xSemaphoreCreateMutex();
			break;

		case IOCTL_REMOTE_SEND_CMD :
			xSemaphoreTake(INSTANCE(aHandle)->mutex ,portMAX_DELAY);

//			tries = 10;
//			while(tries)
//			{
				INSTANCE(aHandle)->transaction_done=0;

				memcpy(packet_buff,
					((struct remote_cmd_t*)aIoctl_param1)->addr_num,SERIAL_NUMBER_LEN);
				memcpy(&packet_buff[SERIAL_NUMBER_LEN],pSerNum,SERIAL_NUMBER_LEN);
				memcpy(&packet_buff[2*SERIAL_NUMBER_LEN],
					((struct remote_cmd_t*)aIoctl_param1)->comand_str,
					((struct remote_cmd_t*)aIoctl_param1)->comand_str_len);

				INSTANCE(aHandle)->currRemoteDeviceAddr =
					((struct remote_cmd_t*)aIoctl_param1)->addr_num;
				INSTANCE(aHandle)->pResult =
					((struct remote_cmd_t*)aIoctl_param1)->result;

				DEV_WRITE( INSTANCE(aHandle)->server_dev ,
						packet_buff,
						(2*SERIAL_NUMBER_LEN) + ((struct remote_cmd_t*)aIoctl_param1)->comand_str_len );

//				count = 1000;
//				while( (0 == INSTANCE(aHandle)->transaction_done) && (count))
//				{
//					vTaskDelay(10);
//					count--;
//				}
//
//				if ( 0 != count) break;

//				tries--;
//			}
			//if (0 == tries) return 2; // transmit failed

			xSemaphoreGive(INSTANCE(aHandle)->mutex );


			break;


		default :
			return 1;
	}
	return 0;
}


uint8_t  remote_devices_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;
	if (usedInstances >= REMOTE_DEVICES_MAX_NUM_OF_GPIOS) return 1;

	REMOTE_DEVICES_InstanceParams[usedInstances].this_dev=aDevDescriptor;

	aDevDescriptor->handle = &REMOTE_DEVICES_InstanceParams[usedInstances];
	aDevDescriptor->ioctl = remote_devices_ioctl;
	aDevDescriptor->callback = remote_devices_callback;

	usedInstances++;

	return 0 ;

}
