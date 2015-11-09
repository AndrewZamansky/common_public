/*
 *
 * file :   BuiltInFlash.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "global_typedefs.h"
#include "BuiltInFlash_API.h"
#include <stdlib.h>
#include <string.h>
#include "BuiltInFlash.h"

/********  defines *********************/


/********  types  *********************/
/********  externals *********************/


/********  local defs *********************/



/**********   external variables    **************/

extern const uint8_t mass_storage_memory[];



/***********   local variables    **************/
static BuiltInFlash_Instance_t BuiltInFlash_InstanceParams = {0} ;


/*******************************************************************************
* Function Name  : BuiltInFlash_API_ReadData
* Description    : Handle the Read operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void  BuiltInFlash_API_ReadData(const void *apHandle,uint32_t startAddr,uint8_t *apData , uint32_t length)
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
* Function Name  : BuiltInFlash_API_WriteData
* Description    : Handle the Write operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void  BuiltInFlash_API_WriteData(const void *apHandle,uint32_t startAddr,const uint8_t *apData , uint32_t length)
{
	// do nothing
}

/*******************************************************************************
* Function Name  : BuiltInFlash_API_ReadData
* Description    : Handle the Read operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint32_t  BuiltInFlash_API_GetBlockCount(BuiltInFlash_API_Handle_t apHandle)
{
	return REPORTED_BLOCKS_ON_STORAGE;
}

/*******************************************************************************
* Function Name  : BuiltInFlash_API_ReadData
* Description    : Handle the Read operation from storage.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint32_t  BuiltInFlash_API_GetBlockSize(BuiltInFlash_API_Handle_t apHandle)
{
	return MASS_STORAGE_MEMORY_BLOCK_SIZE;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BuiltInFlash_API_Init                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  BuiltInFlash_API_Init(BuiltInFlash_API_Init_params_t *apInit_params,BuiltInFlash_API_Handle_t *apHandle)
{
	BuiltInFlash_InstanceParams.chip = apInit_params->chip;

	*apHandle = (BuiltInFlash_API_Handle_t)&BuiltInFlash_InstanceParams;
	return 0;

}
