
#ifndef _I2S_nuc505_static_dev_macros_h_
#define _I2S_nuc505_static_dev_macros_h_

#include "src/I2S_nuc505.h"
uint8_t I2S_nuc505_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);


#define __I2S_NUC505_API_CREATE_STATIC_DEV(pdev,callback_pdev)									\
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(callback_pdev) ;							\
		I2S_NUC505_Instance_t handle_of_##pdev =	 {P_TO_STATIC_DEVICE_INST(callback_pdev)};	\
		STATIC_DEVICE(pdev , &handle_of_##pdev , I2S_nuc505_ioctl , 							\
				DEV_API_dummy_pwrite_func , DEV_API_dummy_pread_func , DEV_API_dummy_callback_func)

#endif
