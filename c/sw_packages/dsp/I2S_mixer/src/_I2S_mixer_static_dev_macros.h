
#ifndef _I2S_mixer_static_dev_macros_h_
#define _I2S_mixer_static_dev_macros_h_

//
//extern uint8_t I2S_mixer_ioctl(pdev_descriptor_t apdev ,const uint8_t aIoctl_num ,void * aIoctl_param1 , void * aIoctl_param2);
//extern size_t I2S_mixer_dsp(const void *aHandle ,const uint8_t *apInData  ,const uint8_t *apOutData ,size_t aLength);
//#include "src/I2S_mixer.h"
//
//
//#define __I2S_MIXER_API_CREATE_STATIC_DEV(dev,dev_name )	\
//		extern const dev_descriptor_t dev ;						\
//	I2S_MIXER_Instance_t handle_of_##dev =	 {0};	\
//	const dev_descriptor_t dev =					\
//		{											\
//			dev_name,								\
//			&handle_of_##dev,						\
//			I2S_mixer_ioctl,						\
//			(dev_pwrite_func_t)I2S_mixer_dsp,		\
//			DEV_API_dummy_pread_func,				\
//			DEV_API_dummy_callback_func				\
//		}
//
//
//



#endif
