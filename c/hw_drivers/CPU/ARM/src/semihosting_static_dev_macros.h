
#ifndef _semihosting_static_dev_macros_h_
#define _semihosting_static_dev_macros_h_


extern uint8_t arm_sh_ioctl(void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1, void * aIoctl_param2);
extern size_t arm_sh_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset);


#define __ARM_SH_API_CREATE_STATIC_DEV(dev,dev_name)	\
		const dev_descriptor_t dev =							\
			{											\
				dev_name,								\
				NULL,									\
				arm_sh_ioctl,							\
				arm_sh_pwrite,							\
				DEV_API_dummy_pread_func,				\
				DEV_API_dummy_callback_func				\
			}


#endif
