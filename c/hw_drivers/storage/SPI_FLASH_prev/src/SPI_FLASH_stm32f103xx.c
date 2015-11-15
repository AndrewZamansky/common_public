/*
 *
 * file :   SPI_FLASH.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "global_typedefs.h"
#include "SPI_FLASH_api.h"
#include <stdlib.h>
#include <string.h>
#include "SPI_FLASH.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

/********  defines *********************/


/********  types  *********************/
/********  externals *********************/


/********  local defs *********************/



/**********   external variables    **************/

/*******************************************************************************
* Function Name  : SPI_FLASH_ChipSelect
* Description    : Selects or deselects the FLASH.
* Input          : State : level to be applied on the FLASH's ChipSelect pin.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_ChipSelect(void)
{
  /* Set High or low the chip select line on PA.4 pin */
  GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)Bit_RESET);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ChipSelect
* Description    : Selects or deselects the FLASH.
* Input          : State : level to be applied on the FLASH's ChipSelect pin.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_ChipDeselect(void)
{
  /* Set High or low the chip select line on PA.4 pin */
  GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)Bit_SET);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
uint8_t SPI_FLASH_SendByte(uint8_t byte)
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

/***********   local variables    **************/
uint8_t SPI_FLASH_STM32F10x_Init(SPI_FLASH_Instance_t *apHandle)
{
	  SPI_InitTypeDef  SPI_InitStructure;
	  GPIO_InitTypeDef GPIO_InitStructure;

	  /* Enable SPI1 and GPIOA clocks */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

	  /* Configure SPI1 pins: NSS, SCK, MISO and MOSI */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  /* Configure PA.4 as Output push-pull, used as Flash Chip select */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  /* Deselect the FLASH: Chip Select high */
	  SPI_FLASH_ChipDeselect();

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
	apHandle->fSendByte = SPI_FLASH_SendByte;
	apHandle->fChipSelect = SPI_FLASH_ChipSelect;
	apHandle->fChipDeselect = SPI_FLASH_ChipDeselect;

	return 0;

}
