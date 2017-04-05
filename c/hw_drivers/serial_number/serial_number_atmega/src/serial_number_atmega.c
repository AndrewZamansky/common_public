/*
 *
 *   file  :  serial_number_atmega.cpp
 *
 */



/***************   includes    *******************/
#include "serial_number_atmega_config.h"
#include "dev_management_api.h" // for device manager defines and typedefs
#include "serial_number_api.h"
#include "src/_serial_number_atmega_prerequirements_check.h" // should be after {uart_atmega_config.h,dev_management_api.h}

#include "serial_number_atmega_api.h"
#include "serial_number_atmega.h"

#include <avr/eeprom.h>

/***************   defines    *******************/


/***************   typedefs    *******************/

/**********   external variables    **************/



/***********   local variables    **************/

static uint8_t serial_number[SERIAL_NUMBER_LEN];
#define SIZE_OF_STAMP	4
static const uint8_t valid_serial_present_stamp[SIZE_OF_STAMP]={0x1A,0xB2,0x3C,0xD4};

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        serial_atmega_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t serial_atmega_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	uint8_t i,val;
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
			for(i=0;i<SIZE_OF_STAMP;i++)
			{
				if(valid_serial_present_stamp[i] != eeprom_read_byte ( (uint8_t *)(i)))
				{
					break;
				}
			}

			if(SIZE_OF_STAMP == i) //wron stamp
			{
				// test if was loaded new serial number
				for(i=SERIAL_NUMBER_LEN ; i<(SIZE_OF_STAMP + SERIAL_NUMBER_LEN) ;i++)
				{
					if(valid_serial_present_stamp[i-SERIAL_NUMBER_LEN] != eeprom_read_byte ( (uint8_t *)(i)))
					{
						break;
					}
				}

				if((SIZE_OF_STAMP + SERIAL_NUMBER_LEN) == i) // new serial not found , load default
				{
					for(i=4;i<SERIAL_NUMBER_LEN;i++)
					{
						eeprom_write_byte ( (uint8_t *)(i) , 0xAF);
					}

				}
				else
				{
					for(i=0;i<SERIAL_NUMBER_LEN;i++)
					{
						eeprom_write_byte ( (uint8_t *)(i) , eeprom_read_byte ( (uint8_t *)(SERIAL_NUMBER_LEN + i)));
					}
				}
			}
			hashed_name_result=hush32_function(CONFIG_PROJECT_NAME,sizeof(CONFIG_PROJECT_NAME));
			memcpy(serial_number,&hashed_name_result,4);
			for(i=4;i<SERIAL_NUMBER_LEN;i++)
			{
				serial_number[i] = eeprom_read_byte ( (uint8_t *)(i));
			}
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
uint8_t  serial_number_atmega_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->handle = NULL;
	aDevDescriptor->ioctl = serial_atmega_ioctl;

	return 0 ;

}
