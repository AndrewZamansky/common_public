/*
 * file : FatFs.h
 *
 *
 */

#ifndef _FatFs_H
#define _FatFs_H

#include "dev_management_api.h"


/***************   typedefs    *******************/

struct FatFs_instance_t{
	struct dev_desc_t *storage_dev;
};

#endif
