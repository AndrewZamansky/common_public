

/*
 *
 * spi_stm8.c
 *
 *
 *
 *
 *
 */

/********  includes *********************/

#include "spi_stm8_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_spi_stm8_prerequirements_check.h" // should be after {spi_stm8_config.h,dev_managment_api.h}

#include "spi_stm8.h"

#include "stm8s.h"

//#include "atom.h"
//#include "atommutex.h"
//#include "spi.h"
#include "stm8s_spi.h"
#include "spi_stm8_api.h"


/********  defines *********************/

#define INSTANCE(hndl)	((SPI_STM8_Instance_t*)hndl)

/********  types  *********************/


/* ---------------------------- External variables ---------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------ Exported variables ------------------------------*/


/********  local defs *********************/
#if SPI_STM8_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0
	SPI_STM8_Instance_t SPI_STM8_Instance;
#endif

/*******************************************************************************
* Function Name  : spi_stm8_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
static uint8_t spi_stm8_SendByte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
	while(SPI_GetFlagStatus( SPI_FLAG_TXE) == RESET);

  /* Send byte through the SPI1peripheral */
	SPI_SendData(byte);

  /* Wait to receive a byte */
	while(SPI_GetFlagStatus( SPI_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_ReceiveData();
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        spi_stm8_pwrite                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
size_t spi_stm8_pread(const void *aHandle , uint8_t *apData , size_t aLength, size_t aOffset)
{
	size_t retVal = aLength;
	while(aLength--)
	{
		*apData=spi_stm8_SendByte(0xff);
		apData++;
	}

	return retVal;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        spi_stm8_pwrite                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
size_t spi_stm8_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset)
{
	size_t retVal = aLength;
	while(aLength--)
	{
		  /* Send byte through the SPI1 peripheral */
		spi_stm8_SendByte( *apData);
		apData++;
	}
	return retVal;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        spi_stm8_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                             						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t spi_stm8_ioctl( void * const aHandle ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(uint8_t*)aIoctl_param2 =   0; //size
			break;

		case IOCTL_DEVICE_START :
			  SPI_DeInit();
			  SPI_Init ( SPI_FIRSTBIT_MSB , SPI_BAUDRATEPRESCALER_2 ,
					  SPI_MODE_MASTER , SPI_CLOCKPOLARITY_LOW ,
					  SPI_CLOCKPHASE_1EDGE , SPI_DATADIRECTION_2LINES_FULLDUPLEX,
					  SPI_NSS_HARD , 0x7);
//			  SPI_Init ( SPI_FIRSTBIT_MSB , SPI_BAUDRATEPRESCALER_2 ,
//					  SPI_MODE_MASTER , SPI_CLOCKPOLARITY_HIGH ,
//					  SPI_CLOCKPHASE_2EDGE , SPI_DATADIRECTION_2LINES_FULLDUPLEX,
//					  SPI_NSS_HARD , 0x7);
			  SPI_Cmd(ENABLE);
			break;
		default :
			return 1;
	}
	return 0;
}

#if SPI_STM8_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        spi_stm8_api_dev_descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  spi_stm8_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->handle = &SPI_STM8_Instance;
	aDevDescriptor->ioctl = spi_stm8_ioctl;
	aDevDescriptor->pwrite = spi_stm8_pwrite;
	aDevDescriptor->pread = spi_stm8_pread
	return 0 ;

}

#endif
