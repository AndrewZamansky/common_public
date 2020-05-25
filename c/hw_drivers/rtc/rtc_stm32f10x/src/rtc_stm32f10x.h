/*
 *
 * rtc_stm32f10x.h
 *
 */

#ifndef _RTC_STM32F10X_H_
#define _RTC_STM32F10X_H_

struct rtc_stm32f10x_runtime_t {
	uint8_t  init_done;
	uint32_t calibration_numerator;
	uint32_t calibration_denumerator;
};
SET_RUNTIME_DATA_TYPE(rtc_stm32f10x, struct rtc_stm32f10x_runtime_t);


#endif
