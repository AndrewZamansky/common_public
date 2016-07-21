/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include "dev_management_api.h"


#ifndef DT_DEV_NAME
	#error "DT_DEV_NAME should be defined"
#endif

#define DEV_INNER_INST(pdev)	DEV_INNER_INST2(pdev)
#define DEV_INNER_INST2(pdev)	inner_inst_##pdev


#ifndef	STATIC_DEV_DATA_STRUCT_TYPE
	#define	HANDLE_TO_DEV_INNER_STRUCT		NULL
#else
	#ifndef STATIC_DEV_DATA_STRUCT
		#error "STATIC_DEV_DATA_STRUCT should be defined"
	#endif

	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(DT_DEV_NAME) ;
	STATIC_DEV_DATA_STRUCT_TYPE	 DEVICE_DATA_PLACEMENT	DEV_INNER_INST(DT_DEV_NAME) = STATIC_DEV_DATA_STRUCT;
	#define	HANDLE_TO_DEV_INNER_STRUCT		&DEV_INNER_INST(DT_DEV_NAME)
#endif


#ifndef	STATIC_DEV_IOCTL_FUNCTION
	#define STATIC_DEV_IOCTL_FUNCTION			DEV_API_dummy_ioctl_func
#endif

#ifndef	STATIC_DEV_PWRITE_FUNCTION
	#define STATIC_DEV_PWRITE_FUNCTION			DEV_API_dummy_pwrite_func
#endif

#ifndef	STATIC_DEV_PREAD_FUNCTION
	#define STATIC_DEV_PREAD_FUNCTION			DEV_API_dummy_pread_func
#endif

#ifndef	STATIC_DEV_CALLBACK_FUNCTION
	#define STATIC_DEV_CALLBACK_FUNCTION		DEV_API_dummy_callback_func
#endif




#define CREATE_STATIC_DEVICE(pdev , dev_handle , dev_ioctl , dev_pwrite , dev_pread , dev_callback)  \
	 CREATE_STATIC_DEVICE2(pdev , dev_handle , dev_ioctl , dev_pwrite , dev_pread , dev_callback)
#define CREATE_STATIC_DEVICE2(pdev , dev_handle , dev_ioctl , dev_pwrite , dev_pread , dev_callback)  \
		DEVICE_PLACEMENT dev_descriptor_t STATIC_DEVICE_INST(pdev) =	\
			{											\
				/*DEVICE_MAGIC_NUMBER,					*/\
				""#pdev,								\
				dev_handle,								\
				dev_ioctl,								\
				dev_pwrite,								\
				dev_pread,								\
				dev_callback							\
			};											\
			pdev_descriptor_t pdev = P_TO_STATIC_DEVICE_INST(pdev)

CREATE_STATIC_DEVICE(DT_DEV_NAME , HANDLE_TO_DEV_INNER_STRUCT ,
		STATIC_DEV_IOCTL_FUNCTION ,  STATIC_DEV_PWRITE_FUNCTION ,
		STATIC_DEV_PREAD_FUNCTION , STATIC_DEV_CALLBACK_FUNCTION);


#undef	DT_DEV_DRIVER
#undef	DT_DEV_NAME
#undef	STATIC_DEV_DATA_STRUCT_TYPE
#undef	STATIC_DEV_DATA_STRUCT
#undef	HANDLE_TO_DEV_INNER_STRUCT
#undef	STATIC_DEV_IOCTL_FUNCTION
#undef	STATIC_DEV_PWRITE_FUNCTION
#undef	STATIC_DEV_PREAD_FUNCTION
#undef	STATIC_DEV_CALLBACK_FUNCTION
