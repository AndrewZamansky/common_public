
#ifndef _RTC_STM32F10X_API_H_
#define _RTC_STM32F10X_API_H_

#include "rtc_api.h"


enum RTC_STM32F10X_API_ioctl_e {
	IOCTL_RTC_STM32F10X_RESERVED = IOCTL_LAST_COMMON_RTC_IOCTL + 1,
};


struct rtc_stm32f10x_config_t {
	struct dev_desc_t  *src_clock;
	uint32_t     rtc_counter_rate_hz;
};
SET_CONFIG_TYPE(rtc_stm32f10x, struct rtc_stm32f10x_config_t);

#endif
