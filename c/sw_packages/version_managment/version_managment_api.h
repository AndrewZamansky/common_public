
#ifndef _version_managment_API_H_
#define _version_managment_API_H_

#include "version_managment_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "src/_version_managment_prerequirements_check.h" // should be after {sw_uart_wrapper_config.h,dev_managment_api.h}

/*****************  defines  **************/


/**********  define API  types ************/



/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt handler function prototype                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

uint8_t  version_managment_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);

#include "src/_version_managment_static_dev_macros.h"

#define VERSION_MANAGMENT_API_CREATE_STATIC_DEV(dev,dev_name ) \
			__VERSION_MANAGMENT_API_CREATE_STATIC_DEV(dev,dev_name )

#endif
