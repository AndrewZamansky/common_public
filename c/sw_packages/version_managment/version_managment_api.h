
#ifndef _version_managment_API_H_
#define _version_managment_API_H_

#include "version_managment_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_version_managment_prerequirements_check.h" // should be after {sw_uart_wrapper_config.h,dev_managment_api.h}

/*****************  defines  **************/

// version pattern :  "yy.mm.dd.hh.mm";
#define START_OF_YEAR_POS  		0
#define START_OF_MONTH_POS 		(START_OF_YEAR_POS	+3)
#define START_OF_DAY_POS 		(START_OF_MONTH_POS	+3)
#define START_OF_HOUR_POS 		(START_OF_DAY_POS	+3)
#define START_OF_MINUTE_POS 	(START_OF_HOUR_POS	+3)

#define VERSION_LENGTH	15
/**********  define API  types ************/



/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt handler function prototype                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

uint8_t  version_managment_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);

#include "src/_version_managment_static_dev_macros.h"

#define VERSION_MANAGMENT_API_CREATE_STATIC_DEV(dev,dev_name ) \
			__VERSION_MANAGMENT_API_CREATE_STATIC_DEV(dev,dev_name )

#endif
