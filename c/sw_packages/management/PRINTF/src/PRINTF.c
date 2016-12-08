/*
 *
 * file :   PRINTF.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "src/_printf_prerequirements_check.h"

#include "PRINTF.h"
#include "PRINTF_api.h"

#include <stdarg.h>

/********  defines *********************/
#define MAX_NUM_OF_PRINTF_INSTANCES 4
#define PRINTF_BUF_LENGTH  128
/********  types  *********************/
typedef struct {
	pdev_descriptor_t dev_descriptor;
} PRINTF_params_t;


/********  externals *********************/


/********  local defs *********************/

#if (CONFIG_USE_MINIMAL_PRINTF)

pdev_descriptor_const  print_dev;

#else

static PRINTF_params_t PRINTF_DebugInstanceParams[MAX_NUM_OF_PRINTF_INSTANCES] = { {0} };
static PRINTF_params_t PRINTF_NoteInstanceParams[MAX_NUM_OF_PRINTF_INSTANCES] = { {0} };

static uint8_t sh_buffer[PRINTF_BUF_LENGTH]; //  define your own buffer’s size


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PRINTF_API_AddDebugOutput                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t PRINTF_API_AddDebugOutput(pdev_descriptor_t aDevHandle)
{
	uint8_t i;

	for(i=0;i< MAX_NUM_OF_PRINTF_INSTANCES; i++)
	{
		if( (NULL == PRINTF_DebugInstanceParams[i].dev_descriptor) ||
				(aDevHandle == PRINTF_DebugInstanceParams[i].dev_descriptor))
		{
			PRINTF_DebugInstanceParams[i].dev_descriptor = aDevHandle;
			return 0;
		}
	}
	return 1;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PRINTF_API_AddNoteOutput                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t PRINTF_API_AddNoteOutput(pdev_descriptor_t aDevHandle )
{
	uint8_t i;

	for(i=0;i< MAX_NUM_OF_PRINTF_INSTANCES; i++)
	{
		if((NULL == PRINTF_NoteInstanceParams[i].dev_descriptor) ||
				(aDevHandle == PRINTF_NoteInstanceParams[i].dev_descriptor))
		{
			PRINTF_NoteInstanceParams[i].dev_descriptor = aDevHandle;
			return 0;
		}
	}
	return 1;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PRINTF_API_RemoveDebugOutput                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t PRINTF_API_RemoveDebugOutput(pdev_descriptor_t aDevHandle)
{
	uint8_t i;

	for(i=0;i< MAX_NUM_OF_PRINTF_INSTANCES; i++)
	{
		if(aDevHandle == PRINTF_DebugInstanceParams[i].dev_descriptor)
		{
			PRINTF_DebugInstanceParams[i].dev_descriptor = NULL;
		}
	}
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PRINTF_API_RemoveNoteOutput                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t PRINTF_API_RemoveNoteOutput(pdev_descriptor_t aDevHandle )
{
	uint8_t i;

	for(i=0;i< MAX_NUM_OF_PRINTF_INSTANCES; i++)
	{
		if(aDevHandle == PRINTF_NoteInstanceParams[i].dev_descriptor)
		{
			PRINTF_NoteInstanceParams[i].dev_descriptor = NULL;
		}
	}
	return 0;
}
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        sendData                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void sendDebugOrNote(const PRINTF_params_t  *PRINTF_params_array,const uint8_t* pBuffer,uint32_t aLen)
{
	uint8_t i;

	for(i=0;i< MAX_NUM_OF_PRINTF_INSTANCES; i++)
	{
		if(NULL != PRINTF_params_array[i].dev_descriptor)
		{
			DEV_WRITE(PRINTF_params_array[i].dev_descriptor,pBuffer , aLen);
		}
	}

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        common_sendData                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void common_sendData(PRINTF_TYPE_t aPrntType , const uint8_t* pBuffer,uint32_t aLen)
{
	switch(aPrntType)
	{
		case PRINTF_TYPE_DBG :
			sendDebugOrNote(PRINTF_DebugInstanceParams,pBuffer,aLen);
			break;
		case PRINTF_TYPE_NOTE :
			sendDebugOrNote(PRINTF_NoteInstanceParams,pBuffer,aLen);
			break;
		default:
			break;
	}
}

#ifndef _DONT_USE_PRINTF
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PRINTF_printf                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void PRINTF_printf(PRINTF_TYPE_t aPrntType , pdev_descriptor_t aDevHandle ,
		   const uint8_t* Format,...)
{
	va_list args;
	int retVal;

	//return ;
	va_start(args,Format);
	retVal=vsnprintf((char*)sh_buffer,PRINTF_BUF_LENGTH,(char*)Format,args);
	va_end(args);

	if(0>=retVal) return ;

	if(NULL != aDevHandle)
	{
		DEV_WRITE(aDevHandle,sh_buffer , retVal);
	}
	else
	{
		common_sendData(aPrntType  ,sh_buffer,retVal);
	}
}

#endif




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PRINTF_print_data                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
void PRINTF_print_data(PRINTF_TYPE_t aPrntType ,  const uint8_t* data,  uint32_t aLen)
{
	if(0>=aLen) return ;

	common_sendData(aPrntType , data ,aLen);


}


#endif
