/*
 *
 *   file  :  serial_number_stm8.cpp
 *
 */


#include "serial_number_stm8_config.h"
#include "dev_management_api.h"
#include "serial_number_api.h"


#include "serial_number_stm8_api.h"
#include "serial_number_stm8.h"


static uint8_t serial_number[SERIAL_NUMBER_LEN];

#if 0
/*******************************************************************************
* Function Name  : IntNumToCharNum.
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void IntNumToCharNum (uint32_t value , uint8_t *pbuf )
{
  uint8_t idx = 0;
  uint8_t currDigit;

  for( idx = 0 ; idx < 8 ; idx ++)
  {
	currDigit= value & 0xf;
    if( currDigit < 0xA )
    {
      pbuf[ idx] = currDigit + '0';
    }
    else
    {
      pbuf[idx] = currDigit + 'A' - 10;
    }

    value = value >> 4;
  }
}
#endif


uint8_t serial_stm8_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	uint32_t hashed_name_result;
	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(uint8_t*)aIoctl_param2 = 0; //size
			break;


		case IOCTL_SERIAL_NUM_GET :
			*(uint8_t **)aIoctl_param1 = serial_number;
			break;
		case IOCTL_DEVICE_START :

			hashed_name_result=hush32_function(CONFIG_PROJECT_NAME,sizeof(CONFIG_PROJECT_NAME));
			memcpy(serial_number,&hashed_name_result,4);
			memcpy(&serial_number[4],(uint8_t*)0x4865,12);
			break;
		default :
			return 1;
	}
	return 0;
}


uint8_t  serial_number_stm8_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->handle = NULL;
	aDevDescriptor->ioctl = serial_stm8_ioctl;

	return 0 ;

}
