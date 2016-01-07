
#ifndef _voice_3D_static_dev_macros_h_
#define _voice_3D_static_dev_macros_h_

//
//extern uint8_t voice_3D_ioctl(void * const aHandle ,const uint8_t aIoctl_num ,void * aIoctl_param1 , void * aIoctl_param2);
//extern size_t voice_3D_dsp(const void *aHandle ,const uint8_t *apInData  ,const uint8_t *apOutData ,size_t aLength);
//#include "src/voice_3D.h"
//
//
//#define __VOICE_3D_API_CREATE_STATIC_DEV(dev,dev_name )	\
//		extern const dev_descriptor_t dev ;						\
//	VOICE_3D_Instance_t handle_of_##dev =	 {0};	\
//	const dev_descriptor_t dev =					\
//		{											\
//			dev_name,								\
//			&handle_of_##dev,						\
//			voice_3D_ioctl,						\
//			(dev_pwrite_func_t)voice_3D_dsp,		\
//			DEV_API_dummy_pread_func,				\
//			DEV_API_dummy_callback_func				\
//		}
//
//
//



#endif
