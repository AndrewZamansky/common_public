/*
 *
 *   file  :  serial_number_stm32f10x.c
 *
 */



/***************   includes    *******************/

#include "serial_number_stm32f10x_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "serial_number_api.h"
#include "src/_serial_number_stm32f10x_prerequirements_check.h" // should be after {uart_stm8_config.h,dev_management_api.h}

#include "serial_number_stm32f10x_api.h"
#include "serial_number_stm32f10x.h"

/***************   defines    *******************/


/***************   typedefs    *******************/

/**********   external variables    **************/



/***********   local variables    **************/

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



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        serial_stm32_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t serial_stm32f10x_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	uint32_t hashed_name_result;


	switch(aIoctl_num)
	{

		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(uint8_t*)aIoctl_param2 =  0; //size
			break;


		case IOCTL_SERIAL_NUM_GET :
			*(uint8_t **)aIoctl_param1 = serial_number;
			break;
		case IOCTL_DEVICE_START :

			hashed_name_result=hush32_function((uint8_t*)CONFIG_PROJECT_NAME,(sizeof(CONFIG_PROJECT_NAME) + 3 ) & 0xfffffffc );
			memcpy(serial_number,&hashed_name_result,4);

			#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD) || defined(STM32L1XX_MD_PLUS)
				memcpy(&serial_number[4],(uint8_t*)0x1FF80050,12);
			#else
				memcpy(&serial_number[4],(uint8_t*)0x1FFFF7E8,12);
			#endif /* STM32L1XX_XD */


			break;

		default :
			return 1;
	}
	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        SerialNumber_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  serial_number_stm32f10x_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->handle = NULL;
	aDevDescriptor->ioctl = serial_stm32f10x_ioctl;

	return 0 ;

}
