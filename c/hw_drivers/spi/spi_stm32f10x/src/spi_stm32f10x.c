/*
 *
 * spi_stm32f10x.c
 *
 *
 *
 *
 *
 */

/********  includes *********************/

#include "spi_stm32f10x_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_spi_stm32f10x_prerequirements_check.h" // should be after {spi_stm32f10x_config.h,dev_managment_api.h}

#include "spi_stm32f10x.h"

#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "spi_stm32f10x_api.h"


/********  defines *********************/

#define INSTANCE(hndl)	((SPI_STM32F10X_Instance_t*)hndl)

/********  types  *********************/


/* ---------------------------- External variables ---------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------ Exported variables ------------------------------*/


/********  local defs *********************/
#if SPI_STM32F10X_CONFIG_NUM_OF_DYNAMIC_INSTANCES > 0
	SPI_STM32F10X_Instance_t SPI_STM32F10X_Instance;
#endif

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
static uint8_t SPI_STM32F10X_SendByte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, byte);

  /* Wait to receive a byte */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        spi_stm32f10x_pwrite                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
size_t spi_stm32f10x_pread(const void *aHandle , uint8_t *apData , size_t aLength, size_t aOffset)
{
	size_t retVal = aLength;
	while(aLength--)
	{
		  /* Send byte through the SPI1 peripheral */
		*apData = SPI_STM32F10X_SendByte(0xFF);
		apData++;
	}

	return retVal;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        spi_stm32f10x_pwrite                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
size_t spi_stm32f10x_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset)
{
	size_t retVal = aLength;
	while(aLength--)
	{

		/* Send byte through the SPI1 peripheral */
		SPI_STM32F10X_SendByte(*apData);

		apData++;
	}
	return retVal;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        spi_stm32f10x_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t spi_stm32f10x_ioctl( void * const aHandle ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(uint8_t*)aIoctl_param2 =   0; //size
			break;

		case IOCTL_DEVICE_START :

			  /* Enable SPI1 and GPIOA clocks */
			  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

			  /* Configure SPI1 pins: NSS, SCK, MISO and MOSI */
			  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
			  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			  GPIO_Init(GPIOA, &GPIO_InitStructure);


			  /* SPI1 configuration */
			  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
			  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
			  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
			  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
			  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
			  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		//	  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
			  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
			  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
			  SPI_InitStructure.SPI_CRCPolynomial = 7;
			  SPI_Init(SPI1, &SPI_InitStructure);

			  /* Enable SPI1  */
			  SPI_Cmd(SPI1, ENABLE);
			  break;
		default :
			return 1;
	}
	return 0;
}

#if SPI_STM32F10X_CONFIG_NUM_OF_DYNAMIC_INSTANCES>0

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        spi_stm32f10x_api_dev_descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  spi_stm32f10x_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->handle = &SPI_STM32F10X_Instance;
	aDevDescriptor->ioctl = spi_stm32f10x_ioctl;
	aDevDescriptor->pwrite = spi_stm32f10x_pwrite;
	aDevDescriptor->pread = spi_stm32f10x_pread;
	return 0 ;

}

#endif
