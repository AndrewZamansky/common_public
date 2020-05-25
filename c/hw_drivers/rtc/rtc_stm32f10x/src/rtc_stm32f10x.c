/*
 *
 *   file  :  rtc_stm32f10x.c
 *
 */

#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"
#include "rtc_api.h"
#include "rtc_stm32f10x_api.h"
#include "rtc_stm32f10x.h"
#include "clock_control_stm32f10x_api.h"
#include "irq_api.h"
#include "os_wrapper.h"

// must be less then 15khz
#define TEST_HZ            10000

// longer callibration period will give better results.
// but too long will spend energy and more then 100 will cause overflow
// 100mSEC of calibration will produce accuracy ~3% of final clock
#define CALIBRATION_DELAY_mSEC  100
#define INTERRUPT_PRIORITY_FOR_RTC     INTERRUPT_PRIORITY_ABOVE_LOWEST(1)



void rtc_stm32f10x_interrupt()
{
	uint16_t status;

	status = RTC->CRL;
	if (RTC_IT_ALR & status)
	{
		// if alarm fired before sleep then reset counter to keep alarm active
		RTC_SetCounter(0);
	}
	else
	{
		CRITICAL_ERROR("TODO");
	}
	status &= ~(RTC_IT_OW | RTC_IT_ALR | RTC_IT_SEC); // clear interrupts
	RTC->CRL = status;
}


static void set_prescaler(struct rtc_stm32f10x_runtime_t *runtime_handle,
							uint32_t freq)
{
	uint32_t rtc_freq;
	uint64_t prescaler_u64;
	uint32_t prescaler_u32;

	DEV_IOCTL_1_PARAMS(stm32f10x_rtc_clk_dev, CLK_IOCTL_GET_FREQ, &rtc_freq);

	// according to datasheet same clock as parent can produce glitches
	if (rtc_freq == freq)
	{
		CRITICAL_ERROR("cannot create requested RTC clock \n");
	}

	// *1024 to increase dynamic range. this will allow create frequencies
	// between (rtc_freq / 2) to rtc_freq
	prescaler_u64 = (rtc_freq * 1024)/ freq;
	prescaler_u64 *= runtime_handle->calibration_numerator;
	prescaler_u64 /= runtime_handle->calibration_denumerator;
	prescaler_u64 /= 1024; // return to original dynamic range
	prescaler_u64--;
	if (0xfffff < prescaler_u64)
	{
		CRITICAL_ERROR("cannot create RTC clock, prescaler too big \n");
	}
	RTC_WaitForLastTask();
	prescaler_u32 = (uint32_t)prescaler_u64;
	RTC_SetPrescaler(prescaler_u32);
}



static void init_rtc(struct rtc_stm32f10x_config_t *config_handle,
					struct rtc_stm32f10x_runtime_t *runtime_handle)
{
	runtime_handle->calibration_denumerator = 1;
	runtime_handle->calibration_numerator = 1;

	PWR_BackupAccessCmd(ENABLE);

	DEV_IOCTL_0_PARAMS(stm32f10x_rtc_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(stm32f10x_rtc_clk_dev,
			CLK_IOCTL_SET_PARENT, config_handle->src_clock);

	// RTC and BKP clock need to be enabled before setting the rate
	DEV_IOCTL_0_PARAMS(stm32f10x_rtc_clk_dev, CLK_IOCTL_ENABLE);
	set_prescaler(runtime_handle, config_handle->rtc_counter_rate_hz);
	//PWR_BackupAccessCmd(DISABLE);

	runtime_handle->init_done = 1;
}


static void set_wakeup(struct rtc_stm32f10x_config_t *config_handle,
						uint32_t wakeup_mSec)
{
	uint64_t alarm_cnt_val;
	uint32_t alarm_cnt_val_u32;

	alarm_cnt_val = config_handle->rtc_counter_rate_hz * wakeup_mSec;
	alarm_cnt_val /= 1000;

	RTC_WaitForLastTask();
	if (0xffffffff < alarm_cnt_val)
	{
		CRITICAL_ERROR("alarm time too big");
	}
	alarm_cnt_val_u32 = (uint32_t)alarm_cnt_val;
	if (0 == alarm_cnt_val_u32)
	{
		CRITICAL_ERROR("alarm time too small");
	}
	RTC_SetAlarm(alarm_cnt_val_u32);
	RTC_WaitForLastTask();
	RTC_SetCounter(0);

	irq_register_interrupt(RTC_IRQn , rtc_stm32f10x_interrupt);
	irq_set_priority(RTC_IRQn , INTERRUPT_PRIORITY_FOR_RTC );
	irq_enable_interrupt(RTC_IRQn);
	RTC_ITConfig(RTC_IT_ALR, ENABLE);
}


/*
 * LSI clock can be between 30khz to 60khz. initial assumption is that its 40khz
 * calibrate to more accurate frequency, using more precise OS clock
 */
static void calibrate(struct rtc_stm32f10x_config_t *config_handle,
						struct rtc_stm32f10x_runtime_t *runtime_handle)
{
	uint32_t result_counter;
	uint32_t expected_results;
	uint32_t calibration_numerator;
	uint32_t calibration_denumerator;

	// cannot be tested on maximal 40khz due to hardware restriction
	set_prescaler(runtime_handle, TEST_HZ);
	RTC_SetCounter(0);
	os_delay_ms(CALIBRATION_DELAY_mSEC);
	result_counter = RTC_GetCounter();
	// ideally we would receive CALIBRATION_DELAY_mSEC * test_hz / 1000
	expected_results = CALIBRATION_DELAY_mSEC * TEST_HZ;
	result_counter *= 1000000;
	calibration_numerator = result_counter / expected_results;
	calibration_denumerator = 1000;

	runtime_handle->calibration_denumerator = calibration_denumerator;
	runtime_handle->calibration_numerator = calibration_numerator;
	set_prescaler(runtime_handle, config_handle->rtc_counter_rate_hz);
}


/**
 * rtc_stm32f10x_ioctl()
 *
 * return:
 */
uint8_t rtc_stm32f10x_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void *aIoctl_param1, void *aIoctl_param2)
{
	struct rtc_stm32f10x_config_t *config_handle;
	struct rtc_stm32f10x_runtime_t *runtime_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(rtc_stm32f10x, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(rtc_stm32f10x, adev);
	if ((0 == runtime_handle->init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR("not initialized yet");
	}

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START:
		init_rtc(config_handle, runtime_handle);
		break;

	case IOCTL_RTC_SET_WAKEUP_mSec:
		set_wakeup(config_handle, *(uint32_t*)aIoctl_param1);
		break;

	case IOCTL_RTC_CALIBRATE:
		calibrate(config_handle, runtime_handle);
		break;

	}
	return 0;
}

#define MODULE_NAME                     rtc_stm32f10x
#define MODULE_IOCTL_FUNCTION           rtc_stm32f10x_ioctl
#include "add_module.h"
