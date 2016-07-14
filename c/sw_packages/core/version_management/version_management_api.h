
#ifndef _version_management_API_H_
#define _version_management_API_H_

#include "src/_version_management_prerequirements_check.h"

/*****************  defines  **************/


/**********  define API  types ************/

extern pdev_descriptor_t ver_dev;


uint8_t  version_management_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);


#endif
