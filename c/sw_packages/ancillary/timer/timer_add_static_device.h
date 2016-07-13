#ifndef _add_static_device_step1
#define _add_static_device_step1

#include  "uart_nuc505_api.h"

#undef _add_static_device_step2

#undef TIMER_DT_DEV_NAME
#undef TIMER_DT_HW_TIMER_PDEV


#elif !defined(_add_static_device_step2)
#define _add_static_device_step2

#undef _add_static_device_step1

#ifndef TIMER_DT_DEV_NAME
#error "TIMER_DT_DEV_NAME should be defined"
#endif

#ifndef TIMER_DT_HW_TIMER_PDEV
#error "TIMER_DT_HW_TIMER_PDEV should be defined"
#endif



extern uint8_t timer_ioctl( void * const aHandle ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#include "src/timer.h"

EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(TIMER_DT_HW_TIMER_PDEV) ;
timer_instance_t STATIC_DEVICE_INNER_INST(TIMER_DT_DEV_NAME)  =
{
	P_TO_STATIC_DEVICE_INST(TIMER_DT_HW_TIMER_PDEV) ,
			0 ,
			0
};
CREATE_STATIC_DEVICE(TIMER_DT_DEV_NAME , &STATIC_DEVICE_INNER_INST(TIMER_DT_DEV_NAME) ,
		timer_ioctl , 	 DEV_API_dummy_pwrite_func ,
		DEV_API_dummy_pread_func , DEV_API_dummy_callback_func);



#undef CURRENT_DEV

#endif
