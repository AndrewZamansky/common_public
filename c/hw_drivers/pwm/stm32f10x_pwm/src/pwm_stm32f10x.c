/*
 *
 *   file  :  pwm_stm32f10x.c
 *
 */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include "dev_management_api.h"

#include "pwm_stm32f10x_api.h"
#include "pin_control_api.h"
#include "clock_control_stm32f10x_api.h"
#include "pwm_stm32f10x.h"
#include "string.h"

static const uint8_t timer_lookup_table_port_A[16] =
	{2, 2, 2, 2, 0xff, 0xff, 3, 3, 1, 1, 1, 1, 1, 0xff, 0xff, 0xff};
static const uint8_t channel_lookup_table_port_A[16] =
	{1, 2, 3, 4, 0xff, 0xff, 1, 2, 1, 2, 3, 4, 5, 0xff, 0xff, 0xff};

static void pwm_init(struct pwm_stm32f10x_instance_t *config_handle,
		struct pwm_stm32f10x_runtime_t *runtime_handle)
{
	DEV_IOCTL_0_PARAMS(stm32f10x_tim1_clk_dev, IOCTL_DEVICE_START);
	DEV_IOCTL_0_PARAMS(stm32f10x_tim1_clk_dev, CLK_IOCTL_ENABLE);
	runtime_handle->init_done = 1;
}


static void set_timer_and_channel(uint32_t  output_pin,
		uint8_t *channel, TIM_TypeDef ** TIMx)
{
	uint8_t  port_num;
	uint8_t  pin_num;
	uint8_t timer_num;
	port_num = (output_pin & PORT_MASK) >> PORT_POS;
	pin_num = (output_pin & PIN_MASK) >> PIN_POS;
	switch (port_num)
	{
	case PORT_A:
		timer_num = timer_lookup_table_port_A[pin_num];
		*channel = channel_lookup_table_port_A[pin_num];
		break;
	default:
		CRITICAL_ERROR("wrong pwm(timer) port");
		break;
	}

	if (0xff == timer_num) CRITICAL_ERROR("wrong pwm(timer) timer");
	if (0xff == *channel) CRITICAL_ERROR("wrong pwm(timer) channel");

	switch(timer_num)
	{
	case 1:
		*TIMx = TIM1;
		break;
	case 2:
		*TIMx = TIM2;
		break;
	case 3:
		*TIMx = TIM3;
		break;
	case 4:
		*TIMx = TIM4;
		break;
	case 5:
		*TIMx = TIM5;
		break;
	case 6:
		*TIMx = TIM6;
		break;
	default:
		CRITICAL_ERROR("not implemented timer");
		break;
	}

}


static uint8_t pwm_set_params(struct pwm_stm32f10x_instance_t *config_handle,
					struct pwm_stm32f10x_runtime_t *runtime_handle,
					struct set_pwm_params * set_params)
{
    TIM_OCInitTypeDef outputChannelInit;
    TIM_TimeBaseInitTypeDef timerInitStructure;
	uint32_t tim1_clock_rate;
	uint8_t channel;
	uint32_t  output_pin;
	TIM_TypeDef* TIMx;
    uint32_t auto_reload_val;
    uint32_t prescaler;
    uint32_t freq;
    uint64_t duty_cycle;

	output_pin = config_handle->output_pin;
	set_timer_and_channel(output_pin, &channel, &TIMx);
	runtime_handle->channel = channel;
	runtime_handle->TIMx = TIMx;

	DEV_IOCTL_1_PARAMS(stm32f10x_tim1_clk_dev,
					CLK_IOCTL_GET_FREQ, &tim1_clock_rate );

	freq = set_params->freq;
	prescaler = 0;
	auto_reload_val = tim1_clock_rate / freq;

	do {
		prescaler++;
		auto_reload_val = (tim1_clock_rate / prescaler) / freq;
	} while (0xffff < auto_reload_val);

	timerInitStructure.TIM_Prescaler = prescaler - 1;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = (uint16_t)auto_reload_val;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIMx, &timerInitStructure);
    TIM_Cmd(TIMx, ENABLE);

    memset(&outputChannelInit, 0, sizeof(TIM_OCInitTypeDef));
    duty_cycle = auto_reload_val;
    duty_cycle *= set_params->duty_cycle_mPercent;
    duty_cycle = duty_cycle / 100000;
    outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
    outputChannelInit.TIM_Pulse = (uint16_t)duty_cycle;
    outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
    outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIMx, &outputChannelInit);
    TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);

    if ((TIM1 == TIMx) || (TIM8 == TIMx))
    {
    	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;
    	TIM_BDTRStructInit(&TIM_BDTRInitStruct);
    	TIM_BDTRConfig(TIMx, &TIM_BDTRInitStruct);
    	TIM_CCPreloadControl(TIMx, ENABLE);
    }

	pin_control_api_set_pin_function(output_pin);
	return 0;
}


static void enable_output(struct pwm_stm32f10x_runtime_t *runtime_handle)
{
	TIM_TypeDef* TIMx;

	TIMx = runtime_handle->TIMx;
    if ((TIM1 == TIMx) || (TIM8 == TIMx))
    {
    	TIM_CtrlPWMOutputs(TIMx, ENABLE);
    }
}


static void disable_output(struct pwm_stm32f10x_runtime_t *runtime_handle)
{
	TIM_TypeDef* TIMx;

	TIMx = runtime_handle->TIMx;
    if ((TIM1 == TIMx) || (TIM8 == TIMx))
    {
    	TIM_CtrlPWMOutputs(TIMx, DISABLE);
    }
}


/*
 * pwm_stm32f10x_ioctl()
 *
 * return:
 */
static uint8_t pwm_stm32f10x_ioctl( struct dev_desc_t *adev,
		const uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct pwm_stm32f10x_instance_t *config_handle;
	struct pwm_stm32f10x_runtime_t *runtime_handle;

	config_handle = DEV_GET_CONFIG_DATA_POINTER(pwm_stm32f10x, adev);
	runtime_handle = DEV_GET_RUNTIME_DATA_POINTER(pwm_stm32f10x, adev);
	if ((0 == runtime_handle->init_done) && (IOCTL_DEVICE_START != aIoctl_num))
	{
		CRITICAL_ERROR("not initialized yet");
	}


	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		pwm_init(config_handle, runtime_handle);
		break;

	case IOCTL_PWM_SET_PARAMS :
		pwm_set_params(config_handle, runtime_handle, aIoctl_param1);
		break;

	case IOCTL_PWM_ENABLE_OUTPUT:
		enable_output(runtime_handle);
		break;

	case IOCTL_PWM_DISABLE_OUTPUT:
		disable_output(runtime_handle);
		break;

	default :
		return 1;
	}
	return 0;
}

#define MODULE_NAME                  pwm_stm32f10x
#define MODULE_IOCTL_FUNCTION        pwm_stm32f10x_ioctl
#include "add_module.h"
