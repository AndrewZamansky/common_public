/*
 *
 * file :   VersionManagment.c
 *
 *
 *
 *
 *
 */



/********  includes *********************/
#include "version_managment_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_version_managment_prerequirements_check.h" // should be after {sw_uart_wrapper_config.h,dev_managment_api.h}

#include "version_managment_api.h"

/********  defines *********************/

#if USE_GCC_BUILD_IN
// Example of __DATE__ string: "Jul 27 2012"
// Example of __TIME__ string: "21:06:19"

const char month[12][2]=
                     {
                    		 "an",
                    		 "eb",
                    		 "ar",
                    		 "pr",
                    		 "ay",
                    		 "un",
                    		 "ul",
                    		 "ug",
                    		 "ep",
                    		 "ct",
                    		 "ov",
                    		 "ec",
                     };
#endif
/********  types  *********************/


/********  externals *********************/


/********  local defs *********************/
//static uint8_t version_str[]="yy.mm.dd.hh.mm";
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        VersionManagment_read                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
size_t version_managment_pread(const void *aHandle ,uint8_t *apData , size_t aLength, size_t aOffset)
{
	if(aLength > sizeof(VERSION_STR))
	{
		aLength = sizeof(VERSION_STR);
	}
	memcpy(apData,VERSION_STR, aLength);
	return aLength;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        ARM_SH_API_Init_Dev_Descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  version_managment_api_init_dev_descriptor(pdev_descriptor aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;

	aDevDescriptor->handle = NULL;
	aDevDescriptor->pread = version_managment_pread;

#if USE_GCC_BUILD_IN
	{
		uint32_t i;

		memcpy(version_str,&__DATE__[9],2);

		for (i=0;i<11 ;i++)
		{
			if (0==memcmp(month[i],&__DATE__[1],2)) break;
		}

		version_str[START_OF_MONTH_POS]= (i>8) ? '1' : '0';
		version_str[START_OF_MONTH_POS+1]= '1' + i;

		version_str[START_OF_DAY_POS] = ((__DATE__[4] >= '0') ? (__DATE__[4]) : '0');
		version_str[START_OF_DAY_POS+1] = __DATE__[5];
		memcpy(&version_str[START_OF_HOUR_POS],&__TIME__[0] ,2);
		memcpy(&version_str[START_OF_MINUTE_POS],&__TIME__[3] ,2);
	}
#endif
	return 0 ;

}
