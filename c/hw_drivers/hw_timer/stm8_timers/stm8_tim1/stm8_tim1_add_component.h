/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "stm8_tim1_api.h"
#include "src/stm8_tim1.h"

uint8_t stm8_tim1_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME						stm8_tim1
#define	MODULE_IOCTL_FUNCTION			stm8_tim1_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE	stm8_tim1_instance_t

#ifdef DT_DEV_MODULE

#if 0
	#ifndef STM8_TIM1_DT_CLOCK_PDEV
		#error "STM8_TIM1_DT_CLOCK_PDEV should be defined"
	#endif

	#ifndef STM8_TIM1_DT_CLOCK_INDEX
		#error "STM8_TIM1_DT_CLOCK_INDEX should be defined"
	#endif
#else
	#ifndef STM8_TIM1_DT_PARENT_CLOCK_RATE
		#error "STM8_TIM1_DT_PARENT_CLOCK_RATE should be defined"
	#endif
#endif
	#ifndef STM8_TIM1_DT_INITIAL_RATE
		#error "STM8_TIM1_DT_INITIAL_RATE should be defined"
	#endif

	#ifndef STM8_TIM1_DT_MODE
		#error "STM8_TIM1_DT_MODE should be defined"
	#endif

	#ifndef STM8_TIM1_DT_CALLBACK
		#define	STM8_TIM1_DT_CALLBACK     NULL
	#else
		void STM8_TIM1_DT_CALLBACK(void);
	#endif

	/*EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(STM8_TIM1_DT_CLOCK_PDEV) ;*/
	#define STATIC_DEV_DATA_STRUCT 	{	\
		STM8_TIM1_DT_INITIAL_RATE ,	\
		STM8_TIM1_DT_PARENT_CLOCK_RATE,	\
		((STM8_TIM1_DT_PARENT_CLOCK_RATE /16 )/ STM8_TIM1_DT_INITIAL_RATE ),	\
		/*P_TO_STATIC_DEVICE_INST(STM8_TIM1_DT_CLOCK_PDEV) ,*/	\
		/*STM8_TIM1_DT_CLOCK_INDEX,*/		\
		STM8_TIM1_DT_CALLBACK ,		\
		STM8_TIM1_DT_MODE  }
#endif

#include "add_static_dev.h"

/* device specific defines should be undefined after calling #include "add_static_dev.h" */
#undef STM8_TIM1_DT_INITIAL_RATE
#undef STM8_TIM1_DT_CLOCK_PDEV
#undef STM8_TIM1_DT_CLOCK_INDEX
#undef STM8_TIM1_DT_PARENT_CLOCK_RATE
#undef STM8_TIM1_DT_MODE
#undef STM8_TIM1_DT_CALLBACK

