/*
 * file : SPI_FLASH.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H


/***************   typedefs    *******************/
typedef enum
{
	SPI_FLASH_Chip_STM32F10x=0,
} SPI_FLASH_Chip_t;

typedef void*  SPI_FLASH_Handle_t;

typedef uint8_t (*pSendByte_func_t)(uint8_t byte)  ;
typedef void (*pChipSelect_func_t)(void)  ;
typedef void (*pChipDeselect_func_t)(void)  ;

typedef struct {
	SPI_FLASH_Chip_t chip;
	pSendByte_func_t fSendByte;
	pChipSelect_func_t fChipSelect;
	pChipDeselect_func_t fChipDeselect;
} SPI_FLASH_Instance_t;

uint8_t SPI_FLASH_STM32F10x_Init(SPI_FLASH_Instance_t *apHandle);

#else
#pragma message( "warning : this header file had already been included" )
#endif /* */
