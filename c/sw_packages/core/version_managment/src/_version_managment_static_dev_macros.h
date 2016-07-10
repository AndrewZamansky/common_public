
#ifndef _version_managment_static_dev_macros_h_
#define _version_managment_static_dev_macros_h_

size_t version_managment_pread(const void *aHandle ,uint8_t *apData , size_t aLength, size_t aOffset);


#define __VERSION_MANAGMENT_API_CREATE_STATIC_DEV(pdev )						\
		STATIC_DEVICE(pdev , NULL , DEV_API_dummy_ioctl_func , 					\
				DEV_API_dummy_pwrite_func , version_managment_pread , DEV_API_dummy_callback_func)


#endif
