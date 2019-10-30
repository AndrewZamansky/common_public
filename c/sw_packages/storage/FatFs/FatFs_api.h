
#ifndef _Fatfs_API_H_
#define _Fatfs_API_H_


#include "dev_management_api.h"
#include "ff.h"


struct FatFs_instance_t {
	struct dev_desc_t *storage_dev;
};
SET_CONFIG_TYPE(FatFs, struct FatFs_instance_t);



#endif
