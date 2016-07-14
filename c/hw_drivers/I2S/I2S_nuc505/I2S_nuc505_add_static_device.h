#ifndef _add_static_device_step1
#define _add_static_device_step1

#include  "I2S_nuc505_api.h"

#undef _add_static_device_step2

#undef I2S_NUC505_DT_DEV_NAME
#undef I2S_NUC505_DT_NUM_OF_WORDS_IN_BUFFER
#undef I2S_NUC505_DT_NUM_OF_BYTES_IN_WORD
#undef I2S_NUC505_DT_CALLBACK_PDEV


#elif !defined(_add_static_device_step2)
#define _add_static_device_step2

#undef _add_static_device_step1

#ifndef I2S_NUC505_DT_DEV_NAME
#error "I2S_NUC505_DT_DEV_NAME should be defined"
#endif

#ifndef I2S_NUC505_DT_NUM_OF_WORDS_IN_BUFFER
#error "I2S_NUC505_DT_NUM_OF_WORDS_IN_BUFFER should be defined"
#endif

#ifndef I2S_NUC505_DT_NUM_OF_BYTES_IN_WORD
#error "I2S_NUC505_DT_NUM_OF_BYTES_IN_WORD should be defined"
#endif

#ifndef I2S_NUC505_DT_CALLBACK_PDEV
#error "I2S_NUC505_DT_CALLBACK_PDEV should be defined"
#endif

#include "src/I2S_nuc505.h"
uint8_t I2S_nuc505_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);


EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(I2S_NUC505_DT_CALLBACK_PDEV) ;
I2S_NUC505_Instance_t STATIC_DEVICE_INNER_INST(I2S_NUC505_DT_DEV_NAME) =
{
		P_TO_STATIC_DEVICE_INST(I2S_NUC505_DT_CALLBACK_PDEV),
		I2S_NUC505_DT_NUM_OF_WORDS_IN_BUFFER,
		I2S_NUC505_DT_NUM_OF_BYTES_IN_WORD
};
CREATE_STATIC_DEVICE(I2S_NUC505_DT_DEV_NAME ,
	&STATIC_DEVICE_INNER_INST(I2S_NUC505_DT_DEV_NAME)  ,
		I2S_nuc505_ioctl ,  DEV_API_dummy_pwrite_func ,
		DEV_API_dummy_pread_func , DEV_API_dummy_callback_func);


#undef CURRENT_DEV

#endif
