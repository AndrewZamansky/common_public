/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "rtc_stm32f10x_api.h"
#include  "src/rtc_stm32f10x.h"


#ifndef RTC_STM32F10X_DT_SRC_CLOCK_DEV
	#error "RTC_STM32F10X_DT_SRC_CLOCK_DEV should be defined"
#endif
EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(RTC_STM32F10X_DT_SRC_CLOCK_DEV);
#define POINTER_TO_SRC_CLOCK_PDEV	 \
			P_TO_STATIC_DEVICE_INST(RTC_STM32F10X_DT_SRC_CLOCK_DEV)


#ifndef RTC_STM32F10X_DT_COUNTER_RATE_HZ
	#error "RTC_STM32F10X_DT_COUNTER_RATE_HZ should be defined"
#endif



SET_STATIC_DEV_CONFIG(rtc_stm32f10x) =
{
	POINTER_TO_SRC_CLOCK_PDEV,
	RTC_STM32F10X_DT_COUNTER_RATE_HZ
};


#include "add_static_dev.h"

/* device specific defines should be undefined
 * after calling #include "add_static_dev.h"
 */
#undef RTC_STM32F10X_DT_SRC_CLOCK_DEV
#undef POINTER_TO_SRC_CLOCK_PDEV
#undef RTC_STM32F10X_DT_COUNTER_RATE_HZ
