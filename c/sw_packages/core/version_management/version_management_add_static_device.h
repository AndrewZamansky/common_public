#ifndef _add_static_device_step1
#define _add_static_device_step1

#include  "version_management_api.h"

#undef _add_static_device_step2

#undef VERSION_MANAGEMENT_DT_DEV_NAME


#elif !defined(_add_static_device_step2)
#define _add_static_device_step2

#undef _add_static_device_step1

#ifndef VERSION_MANAGEMENT_DT_DEV_NAME
#error "VERSION_MANAGEMENT_DT_DEV_NAME should be defined"
#endif


size_t version_management_pread(const void *aHandle ,uint8_t *apData , size_t aLength, size_t aOffset);


CREATE_STATIC_DEVICE(VERSION_MANAGEMENT_DT_DEV_NAME , NULL , DEV_API_dummy_ioctl_func ,
		DEV_API_dummy_pwrite_func , version_management_pread , DEV_API_dummy_callback_func);



#undef CURRENT_DEV

#endif
