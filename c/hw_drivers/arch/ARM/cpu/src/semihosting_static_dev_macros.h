
#ifndef _semihosting_static_dev_macros_h_
#define _semihosting_static_dev_macros_h_


#include "src/semihosting.h"
extern uint8_t arm_sh_ioctl(void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1, void * aIoctl_param2);
extern size_t arm_sh_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);

#define __ARM_SH_API_CREATE_STATIC_DEV(pdev ,callback_pdev)								\
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(pdev) ;								\
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(callback_pdev) ;						\
		SEMIHOSTING_Instance_t handle_of_##dev =	 									\
			{P_TO_STATIC_DEVICE_INST(callback_pdev),P_TO_STATIC_DEVICE_INST(pdev)};		\
		STATIC_DEVICE(pdev , &handle_of_##pdev , arm_sh_ioctl , 						\
				arm_sh_pwrite , DEV_API_dummy_pread_func , DEV_API_dummy_callback_func)


#endif
