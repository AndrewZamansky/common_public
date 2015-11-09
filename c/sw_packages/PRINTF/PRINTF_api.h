
#ifndef _PRINTF_API_H_
#define _PRINTF_API_H_

#include "printf_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_printf_prerequirements_check.h" // should be after {printf_config.h,dev_managment_api.h}

/*****************  defines  **************/


/**********  define API  types ************/



/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt handler function prototype                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

typedef enum
{
	PRINTF_TYPE_DBG,
	PRINTF_TYPE_NOTE,
	PRINTF_TYPE_REPLY

} PRINTF_TYPE_t;

uint32_t PRINTF_API_AddDebugOutput(pdev_descriptor_const aDevHandle);
uint32_t PRINTF_API_AddNoteOutput(pdev_descriptor_const aDevHandle);
uint32_t PRINTF_API_RemoveDebugOutput(pdev_descriptor_const aDevHandle);
uint32_t PRINTF_API_RemoveNoteOutput(pdev_descriptor_const aDevHandle);

void PRINTF_printf(PRINTF_TYPE_t aPrntType , pdev_descriptor_const aDevHandle,  const uint8_t* Format,...);
void PRINTF_print_data(PRINTF_TYPE_t aPrntType , const uint8_t* data,  uint32_t aLen);


/**********  define API  functions  ************/
#if (PRINTF_CONFIG_USE_MINIMAL)

 extern pdev_descriptor_const  print_dev;

 #define PRINTF_API_SET_DEV(dev)				pdev_descriptor_const  print_dev = dev
 #define PRINT_DATA_DBG(data,len) 				DEV_WRITE(print_dev,data , len) // to avoid printf code
 #define PRINT_STR_DBG(str) 					DEV_WRITE(print_dev,(uint8_t*)str, strlen((char*)str)) // to avoid printf code

#else

 #define PRINTF_DBG(...) 						PRINTF_printf(PRINTF_TYPE_DBG	,NULL 	,(uint8_t*)__VA_ARGS__)
 #define PRINTF_NOTE(...) 						PRINTF_printf(PRINTF_TYPE_NOTE	,NULL 	,(uint8_t*)__VA_ARGS__)
 #define PRINTF_REPLY(dev_desc,...) 				PRINTF_printf(PRINTF_TYPE_REPLY	,dev_desc,(uint8_t*)__VA_ARGS__)

 #define PRINT_STR_DBG(str) 						PRINTF_print_data(PRINTF_TYPE_DBG	 ,(uint8_t*)str, strlen((char*)str)) // to avoid printf code
 #define PRINT_STR_NOTE(str) 					PRINTF_print_data(PRINTF_TYPE_NOTE	 ,(uint8_t*)str, strlen((char*)str)) // to avoid printf code
 #define PRINT_STR_REPLY(dev_desc,str) 			DEV_WRITE(dev_desc, (const uint8_t*)str , strlen((char*)str))// to avoid printf code

 #define PRINT_DATA_DBG(data,len) 				PRINTF_print_data(PRINTF_TYPE_DBG  ,(uint8_t*)data,len) // to avoid printf code
 #define PRINT_DATA_NOTE(data,len) 				PRINTF_print_data(PRINTF_TYPE_NOTE ,(uint8_t*)data,len) // to avoid printf code
 #define PRINT_DATA_REPLY(dev_desc,data,len) 	DEV_WRITE(dev_desc,(const uint8_t*)data , len)// to avoid printf code

#endif


#endif
