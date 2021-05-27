
#ifndef _adc_stm8_API_H_
#define _adc_stm8_API_H_

#include "src/_adc_stm8_prerequirements_check.h"
#include "adc_api.h"

enum adc_stm8_api_channel_e {
  ADC_STM8_API_CHANNEL_0  = (uint8_t)0x00, /**< Analog channel 0 */
  ADC_STM8_API_CHANNEL_1  = (uint8_t)0x01, /**< Analog channel 1 */
  ADC_STM8_API_CHANNEL_2  = (uint8_t)0x02, /**< Analog channel 2 */
  ADC_STM8_API_CHANNEL_3  = (uint8_t)0x03, /**< Analog channel 3 */
  ADC_STM8_API_CHANNEL_4  = (uint8_t)0x04, /**< Analog channel 4 */
  ADC_STM8_API_CHANNEL_5  = (uint8_t)0x05, /**< Analog channel 5 */
  ADC_STM8_API_CHANNEL_6  = (uint8_t)0x06, /**< Analog channel 6 */
  ADC_STM8_API_CHANNEL_7  = (uint8_t)0x07, /**< Analog channel 7 */
  ADC_STM8_API_CHANNEL_8  = (uint8_t)0x08, /**< Analog channel 8 */
  ADC_STM8_API_CHANNEL_9  = (uint8_t)0x09, /**< Analog channel 9 */
  ADC_STM8_API_CHANNEL_12 = (uint8_t)0x0C /**< Analog channel 12 */
     /* refer to product datasheet for channel 12 availability */
};

#endif
