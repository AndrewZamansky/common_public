
#ifndef _semihosting_static_dev_macros_h_
#define _semihosting_static_dev_macros_h_


#include "src/semihosting.h"
extern uint8_t arm_sh_ioctl(void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1, void * aIoctl_param2);
extern size_t arm_sh_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);

#define __ARM_SH_API_CREATE_STATIC_DEV(dev,dev_name,callback_dev)	\
		extern const dev_descriptor_t callback_dev ;			\
		SEMIHOSTING_Instance_t handle_of_##dev =	 {&callback_dev};	\
		const dev_descriptor_t dev =							\
			{											\
				dev_name,								\
				&handle_of_##dev,						\
				arm_sh_ioctl,							\
				arm_sh_pwrite,							\
				DEV_API_dummy_pread_func,				\
				DEV_API_dummy_callback_func				\
			}


#endif
