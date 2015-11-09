
#ifndef _BuiltInFlash_API_H_
#define _BuiltInFlash_API_H_

/*****************  defines  **************/


/**********  define API  types ************/

typedef void*  BuiltInFlash_API_Handle_t;

typedef enum
{
	BuiltInFlash_API_Chip_STM32F10x=0,
} BuiltInFlash_API_Chip_t;

typedef struct {
	BuiltInFlash_API_Chip_t chip;
} BuiltInFlash_API_Init_params_t;


/**********  define API  functions  ************/
uint8_t  BuiltInFlash_API_Init(BuiltInFlash_API_Init_params_t *apInit_params,BuiltInFlash_API_Handle_t *apHandle);

void  BuiltInFlash_API_ReadData(const void *apHandle,uint32_t startAddr, uint8_t *apData , uint32_t length);
void  BuiltInFlash_API_WriteData(const void *apHandle,uint32_t startAddr,const uint8_t *apData , uint32_t length);

uint32_t  BuiltInFlash_API_GetBlockCount(BuiltInFlash_API_Handle_t apHandle);
uint32_t  BuiltInFlash_API_GetBlockSize(BuiltInFlash_API_Handle_t apHandle);



#else
#pragma message( "warning : this header file had already been included" )
#endif
