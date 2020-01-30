
#ifndef _adc_stm32f10x_API_H_
#define _adc_stm32f10x_API_H_


struct adc_stm32f10x_config_t {
	uint32_t adc_clock_rate;
	uint8_t channel;
};
SET_CONFIG_TYPE(adc_stm32f10x, struct adc_stm32f10x_config_t);


#endif
