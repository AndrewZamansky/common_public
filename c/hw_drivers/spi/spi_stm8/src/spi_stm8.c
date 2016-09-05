

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

#include "src/_spi_stm8_prerequirements_check.h" // should be after {spi_stm8_config.h,dev_management_api.h}

#include "spi_stm8.h"

#include "stm8s.h"

//#include "atom.h"
//#include "atommutex.h"
//#include "spi.h"
#include "stm8s_spi.h"
#include "spi_stm8_api.h"
#include "spi_stm8_add_component.h"


/********  defines *********************/

#define INSTANCE(hndl)	((SPI_STM8_Instance_t*)hndl)

/********  types  *********************/


/* ---------------------------- External variables ---------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------ Exported variables ------------------------------*/


/********  local defs *********************/



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
size_t spi_stm8_pread(pdev_descriptor_t apdev , uint8_t *apData , size_t aLength, size_t aOffset)
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
size_t spi_stm8_pwrite(pdev_descriptor_t apdev ,const uint8_t *apData , size_t aLength, size_t aOffset)
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
uint8_t spi_stm8_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{

	switch(aIoctl_num)
	{
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
