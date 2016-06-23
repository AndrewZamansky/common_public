
#ifndef _version_managment_API_H_
#define _version_managment_API_H_

#include "src/_version_managment_prerequirements_check.h"

/*****************  defines  **************/


/**********  define API  types ************/

extern pdev_descriptor_t ver_dev;


uint8_t  version_managment_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);

#include "src/_version_managment_static_dev_macros.h"

#define VERSION_MANAGMENT_API_CREATE_STATIC_DEV(pdev ) \
			__VERSION_MANAGMENT_API_CREATE_STATIC_DEV(pdev )

#endif
