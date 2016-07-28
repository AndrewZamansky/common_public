/*
 *
 * file :   ram_disk.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "_project.h"
#include "ram_disk_API.h"
#include <stdlib.h>
#include <string.h>
#include "ram_disk.h"

/********  defines *********************/


/********  types  *********************/
/********  externals *********************/


/********  local defs *********************/



/**********   external variables    **************/

extern const uint8_t mass_storage_memory[];



/***********   local variables    **************/
static ram_disk_Instance_t ram_disk_InstanceParams = {0} ;


/*******************************************************************************
* Function Name  : ram_disk_API_ReadData
* Description    : Handle the Read operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void  ram_disk_API_ReadData(const void *apHandle,uint32_t startAddr,uint8_t *apData , uint32_t length)
{

	if (startAddr >= REAL_MASS_STORAGE_MEMORY_SIZE)
	{
		memset(apData,0,length);
		return ;
	}

	if  ((startAddr + length)>REAL_MASS_STORAGE_MEMORY_SIZE)
	{
		length = REAL_MASS_STORAGE_MEMORY_SIZE - startAddr;
	}

	memcpy(apData,&mass_storage_memory[startAddr],length);
}


/*******************************************************************************
* Function Name  : ram_disk_API_WriteData
* Description    : Handle the Write operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void  ram_disk_API_WriteData(const void *apHandle,uint32_t startAddr,const uint8_t *apData , uint32_t length)
{
	// do nothing
}

/*******************************************************************************
* Function Name  : ram_disk_API_ReadData
* Description    : Handle the Read operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint32_t  ram_disk_GetBlockCount(ram_disk_API_Handle_t apHandle)
{
	return REPORTED_BLOCKS_ON_STORAGE;
}

/*******************************************************************************
* Function Name  : ram_disk_API_ReadData
* Description    : Handle the Read operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint32_t  ram_disk_GetBlockSize(ram_disk_API_Handle_t apHandle)
{
	return MASS_STORAGE_MEMORY_BLOCK_SIZE;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ram_disk_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t ram_disk_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	switch(aIoctl_num)
	{

		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(uint8_t*)aIoctl_param2 =   0; //size
			break;

		case IOCTL_DEVICE_START :
			/* Deselect the FLASH: Chip Select high */
			DEV_IOCTL_0_PARAMS(INSTANCE(aHandle)->gpio_select_dev , IOCTL_GPIO_PIN_SET );
//			{ // for spi test
//				uint32_t tmp=SPI_FLASH_ReadID(aHandle);
//			}

			break;

		case IOCTL_SPI_FLASH_ERRASE_ALL :
			SPI_FLASH_BulkErase(aHandle);
			break;

		case IOCTL_SPI_FLASH_ERRASE_SECTOR :
			SPI_FLASH_SectorErase(aHandle,*(uint32_t*)aIoctl_param1);
			break;

		default :
			return 1;
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ram_disk_API_Init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  ram_disk_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor)
{
	ram_disk_InstanceParams.chip = apInit_params->chip;

	*apHandle = (ram_disk_API_Handle_t)&ram_disk_InstanceParams;
	return 0;

}
