
#ifndef _PRINTF_API_H_
#define _PRINTF_API_H_

#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

/*****************  defines  **************/


/**********  define API  types ************/


typedef enum
{
	PRINTF_TYPE_DBG,
	PRINTF_TYPE_NOTE,
	PRINTF_TYPE_REPLY
} PRINTF_TYPE_t;

uint8_t PRINTF_API_AddDebugOutput(struct dev_desc_t *aDevHandle);
uint8_t PRINTF_API_AddNoteOutput(struct dev_desc_t *aDevHandle);
uint8_t PRINTF_API_RemoveDebugOutput(struct dev_desc_t *aDevHandle);
uint8_t PRINTF_API_RemoveNoteOutput(struct dev_desc_t *aDevHandle);

void PRINTF_printf(PRINTF_TYPE_t aPrntType, const char* Format, ...);
void PRINTF_REPLY(struct dev_desc_t *aDevHandle, const char* Format, ...);

void PRINTF_print_data(
		PRINTF_TYPE_t aPrntType, const uint8_t* data,  uint32_t aLen);
void PRINT_DATA_REPLY(
		struct dev_desc_t *aDevHandle, const uint8_t* data, uint32_t aLen);

void PRINTF_print_str(PRINTF_TYPE_t aPrntType, const char* str);
void PRINT_STR_REPLY(struct dev_desc_t *aDevHandle, const char* str);

/**********  define API  functions  ************/
#if (CONFIG_USE_MINIMAL_PRINTF)

 extern struct dev_desc_t const *print_dev;

 #define PRINTF_API_SET_DEV(dev)	print_dev = dev
 #define PRINT_DATA_DBG(data,len) 	DEV_WRITE(print_dev,data , len)
 #define PRINT_STR_DBG(str) 		\
					DEV_WRITE(print_dev,(uint8_t*)str, strlen((char*)str))

#else

 #define PRINTF_DBG(...)   PRINTF_printf(PRINTF_TYPE_DBG, __VA_ARGS__)
 #define PRINTF_NOTE(...)  PRINTF_printf(PRINTF_TYPE_NOTE, __VA_ARGS__)

 // use following functions to avoid  printf  code
 #define PRINT_STR_DBG(str) 		 PRINTF_print_str(PRINTF_TYPE_DBG, str)
 #define PRINT_STR_NOTE(str) 		 PRINTF_print_str(PRINTF_TYPE_NOTE, str)

 #define PRINT_DATA_DBG(data, len) 	   \
				PRINTF_print_data(PRINTF_TYPE_DBG, (uint8_t*)data, len)
 #define PRINT_DATA_NOTE(data, len) 	\
				PRINTF_print_data(PRINTF_TYPE_NOTE, (uint8_t*)data, len)

#endif


#if !defined(CONFIG_INCLUDE_PRINTF) || 	\
			defined(CONFIG_DISABLE_DBG_PRINTS) || !defined(DEBUG)
 	 #define __DISABLE_DBG_PRINTS
#endif


#ifdef __DISABLE_DBG_PRINTS

	#undef PRINTF_DBG
	#undef PRINT_STR_DBG
	#undef PRINT_DATA_DBG

	#define PRINTF_DBG(...)
	#define PRINT_STR_DBG(str)
	#define PRINT_DATA_DBG(data,len)

#endif

#endif
