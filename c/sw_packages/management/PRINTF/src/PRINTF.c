/*
 *
 * file :   PRINTF.c
 *
 *
 */



/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "PRINTF.h"
#include "PRINTF_api.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#if defined(COMPILING_FOR_LINUX_HOST)
	#include <unistd.h>
#endif
/********  defines *********************/
#define MAX_NUM_OF_PRINTF_INSTANCES 4
#define PRINTF_BUF_LENGTH  128
/********  types  *********************/


/********  externals *********************/


/********  local defs *********************/

#if (CONFIG_USE_MINIMAL_PRINTF)

struct dev_desc_t const *print_dev = NULL;

#else

static struct dev_desc_t *debug_out_devs[MAX_NUM_OF_PRINTF_INSTANCES] = {NULL};
static struct dev_desc_t *note_out_devs[MAX_NUM_OF_PRINTF_INSTANCES] = {NULL};

static uint8_t sh_buffer[PRINTF_BUF_LENGTH]; //  define your own buffer size


static uint8_t add_dev(
		struct dev_desc_t  **out_devs, struct dev_desc_t *aDevHandle)
{
	uint8_t i;

	for (i = 0; i < MAX_NUM_OF_PRINTF_INSTANCES; i++)
	{
		if ( (NULL == out_devs[i]) || (aDevHandle == out_devs[i]) )
		{
			out_devs[i] = aDevHandle;
			return 0;
		}
	}
	return 1;
}


/**
 * PRINTF_API_AddDebugOutput()
 *
 * return:
 */
uint8_t PRINTF_API_AddDebugOutput(struct dev_desc_t *aDevHandle)
{
	return add_dev(debug_out_devs, aDevHandle);
}


/**
 * PRINTF_API_AddNoteOutput()
 *
 * return:
 */
uint8_t PRINTF_API_AddNoteOutput(struct dev_desc_t *aDevHandle )
{
	return add_dev(note_out_devs, aDevHandle);
}




static uint8_t remove_dev(
		struct dev_desc_t  **out_devs, struct dev_desc_t *aDevHandle)
{
	uint8_t i;

	for (i = 0; i < MAX_NUM_OF_PRINTF_INSTANCES; i++)
	{
		if(aDevHandle == out_devs[i])
		{
			out_devs[i] = NULL;
		}
	}
	return 0;
}


/**
 * PRINTF_API_RemoveDebugOutput()
 *
 * return:
 */
uint8_t PRINTF_API_RemoveDebugOutput(struct dev_desc_t *aDevHandle)
{
	return remove_dev(debug_out_devs, aDevHandle);
}


/**
 * PRINTF_API_RemoveNoteOutput()
 *
 * return:
 */
uint8_t PRINTF_API_RemoveNoteOutput(struct dev_desc_t *aDevHandle )
{
	return remove_dev(note_out_devs, aDevHandle);
}


/**
 * sendDebugOrNote()
 *
 * return:
 */
static void sendDebugOrNote(struct dev_desc_t  **out_devs,
		const uint8_t* pBuffer,uint32_t aLen)
{
	uint8_t i;

	for (i = 0; i < MAX_NUM_OF_PRINTF_INSTANCES; i++)
	{
		if (NULL != out_devs[i])
		{
			DEV_WRITE(out_devs[i], pBuffer, aLen);
		}
	}
}


/**
 * common_sendData()
 *
 * return:
 */
static void common_sendData(
		enum PRINTF_TYPE_e aPrntType, const uint8_t* pBuffer, uint32_t aLen)
{
	switch(aPrntType)
	{
		case PRINTF_TYPE_DBG :
			sendDebugOrNote(debug_out_devs,pBuffer,aLen);
			#if defined(COMPILING_FOR_HOST)
				write(1, pBuffer, aLen);
			#endif
			break;
		case PRINTF_TYPE_NOTE :
			sendDebugOrNote(note_out_devs,pBuffer,aLen);
			break;
		default:
			break;
	}
}


#ifndef _DONT_USE_PRINTF
/**
 * PRINTF_printf()
 *
 * return:
 */
void PRINTF_printf(enum PRINTF_TYPE_e aPrntType, const char* Format, ...)
{
	va_list args;
	int retVal;

	va_start(args, Format);
	retVal = vsnprintf(
			(char*)sh_buffer, PRINTF_BUF_LENGTH, Format, args);
	va_end(args);

	if (0 >= retVal) return ;

	common_sendData(aPrntType, sh_buffer, retVal);
}


#endif




/**
 * PRINTF_print_data()
 *
 * return:
 */
void PRINTF_print_data(
		enum PRINTF_TYPE_e aPrntType, const uint8_t* data, uint32_t aLen)
{
	if (0 == aLen) return ;
	common_sendData(aPrntType, data, aLen);
}


/**
 * PRINTF_print_str()
 *
 * return:
 */
void PRINTF_print_str(enum PRINTF_TYPE_e aPrntType, const char* str)
{
	common_sendData(aPrntType, (uint8_t*)str, strlen(str));
}


#endif
