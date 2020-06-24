
#ifndef _adc_stm32f10x_API_H_
#define _adc_stm32f10x_API_H_

#define ADC_STM32_F103X_VIRTUAL_TEMPERATURE_PIN  0x260
#define ADC_STM32_F103X_VIRTUAL_VREF_PIN         0x270

struct adc_stm32f10x_config_t {
	uint32_t adc_clock_rate;
	uint32_t input_pin;
};
SET_CONFIG_TYPE(adc_stm32f10x, struct adc_stm32f10x_config_t);


#endif
