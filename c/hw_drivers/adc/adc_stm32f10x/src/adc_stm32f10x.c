/*
 *
 *   file  :  ADC.cpp
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h" // for device manager defines and typedefs

#include "adc_api.h"
#include "adc_stm32f10x_api.h"
#include "adc_stm32f10x.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "pin_control_api.h"
#include "dt_pin_control_stm32f10x.h"
#include "clock_control_stm32f10x_api.h"
#include "os_wrapper.h"

static void init_adc(struct dev_desc_t *adev,
		struct adc_stm32f10x_config_t *config_handle,
		struct adc_stm32f10x_runtime_t *runtime_handle)
{
	ADC_InitTypeDef lADC_InitStruct;
	uint8_t channel;
	uint32_t  pin_control;
	uint8_t  port_num;
	uint8_t  pin_num;

	channel = config_handle->channel;
	port_num = PORT_A;
	pin_num = channel;
	if (9 < channel)
	{
		port_num = PORT_C;
		pin_num = channel - 10;
	}
	else if (7 < channel)
	{
		port_num = PORT_B;
		pin_num = channel - 8;
	}

	// channel 16 it's internal ADC for temperature, so no pin
	// channel 17 is for Vref
	if (16 > channel)
	{
		pin_control = FILL_PIN_DATA(
			port_num, pin_num, 0, DUMMY_AF_REMAP_REG, 0, 0, 0);
		pin_control_api_set_pin_function(pin_control);
	}

	DEV_IOCTL_0_PARAMS(stm32f10x_adc_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_1_PARAMS(stm32f10x_adc_clk_dev,
			CLK_IOCTL_SET_FREQ, &config_handle->adc_clock_rate);
	DEV_IOCTL_0_PARAMS(stm32f10x_adc_clk_dev, CLK_IOCTL_ENABLE);

	ADC_DeInit(ADC1);

	ADC_StructInit(&lADC_InitStruct);
	lADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_Init(ADC1, &lADC_InitStruct);

	ADC_Cmd(ADC1, ENABLE); // first write to ADC1 will power-up adc

	ADC_ResetCalibration(ADC1);
	while (SET == ADC_GetResetCalibrationStatus(ADC1)) {}
	ADC_StartCalibration(ADC1);
	while (SET == ADC_GetCalibrationStatus(ADC1)) {}

	runtime_handle->init_done = 1;
}


/**
 * adc_stm32f10x_ioctl()
 *
 * return:
 */
uint8_t adc_stm32f10x_ioctl(struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct adc_stm32f10x_config_t *config_handle;
	struct adc_stm32f10x_runtime_t *runtime_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(adc_stm32f10x, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(adc_stm32f10x, adev);
	if ((0 == runtime_handle->init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR("not initialized yet");
	}

	switch(aIoctl_num)
	{
	case IOCTL_ADC_GET_CURRENT_VALUE_mV :
		ADC_RegularChannelConfig(
				ADC1, config_handle->channel, 1, ADC_SampleTime_239Cycles5);
		ADC_Cmd(ADC1, ENABLE);

		while (RESET == ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
		{
			//os_delay_ms(1); measured conversion is taking ~70us
			// (~11 loops on 8Mhz cpu clock)
		}

		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

		// 3.3v = 3300mv. 0xfff because 12bit ADC
		*(uint32_t*)aIoctl_param1 =
				(3300 * (uint32_t)ADC_GetConversionValue(ADC1)) / 0xfff;
		break;

	case IOCTL_DEVICE_START :
		init_adc(adev, config_handle, runtime_handle);
		break;
	}
	return 0;
}

#define MODULE_NAME              adc_stm32f10x
#define MODULE_IOCTL_FUNCTION    adc_stm32f10x_ioctl

#define MODULE_CONFIGURABLE_PARAMS_ARRAY  {     \
		{IOCTL_ADC_STM32F10X_SET_CHANNEL_PARAM , IOCTL_VOID ,\
				ADC_STM32F10X_API_CHANNEL_STR, NOT_FOR_SAVE} \
	}

#include "add_module.h"
