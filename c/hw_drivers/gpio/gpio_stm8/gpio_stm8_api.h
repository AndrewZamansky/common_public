
#ifndef _GPIO_STM8_API_H_
#define _GPIO_STM8_API_H_

#include "src/_gpio_stm8_prerequirements_check.h"
#include "gpio_api.h"


#define GPIO_STM8_API_PORT_STR   	"port"
#define GPIO_STM8_API_PIN_STR   	"pin"
#define GPIO_STM8_API_MODE_STR   	"mode"



enum GPIO_STM8_API_ioctl_e {
	IOCTL_GPIO_STM8_SET_PORT_PARAM = IOCTL_LAST_COMMON_GPIO_IOCTL+1,
	IOCTL_GPIO_STM8_SET_PIN_PARAM,
	IOCTL_GPIO_STM8_SET_MODE_PARAM
};


enum GPIO_STM8_API_Mode_e {
  GPIO_STM8_API_MODE_IN_FL_NO_IT      = (uint8_t)0x00,  /*!< Input floating, no external interrupt */
  GPIO_STM8_API_MODE_IN_PU_NO_IT      = (uint8_t)0x40,  /*!< Input pull-up, no external interrupt */
  GPIO_STM8_API_MODE_IN_FL_IT         = (uint8_t)0x20,  /*!< Input floating, external interrupt */
  GPIO_STM8_API_MODE_IN_PU_IT         = (uint8_t)0x60,  /*!< Input pull-up, external interrupt */
  GPIO_STM8_API_MODE_OUT_OD_LOW_FAST  = (uint8_t)0xA0,  /*!< Output open-drain, low level, 10MHz */
  GPIO_STM8_API_MODE_OUT_PP_LOW_FAST  = (uint8_t)0xE0,  /*!< Output push-pull, low level, 10MHz */
  GPIO_STM8_API_MODE_OUT_OD_LOW_SLOW  = (uint8_t)0x80,  /*!< Output open-drain, low level, 2MHz */
  GPIO_STM8_API_MODE_OUT_PP_LOW_SLOW  = (uint8_t)0xC0,  /*!< Output push-pull, low level, 2MHz */
  GPIO_STM8_API_MODE_OUT_OD_HIZ_FAST  = (uint8_t)0xB0,  /*!< Output open-drain, high-impedance level,10MHz */
  GPIO_STM8_API_MODE_OUT_PP_HIGH_FAST = (uint8_t)0xF0,  /*!< Output push-pull, high level, 10MHz */
  GPIO_STM8_API_MODE_OUT_OD_HIZ_SLOW  = (uint8_t)0x90,  /*!< Output open-drain, high-impedance level, 2MHz */
  GPIO_STM8_API_MODE_OUT_PP_HIGH_SLOW = (uint8_t)0xD0   /*!< Output push-pull, high level, 2MHz */
};

enum GPIO_STM8_API_Pin_e {
  GPIO_STM8_API_PIN_0    = ((uint8_t)0x01),  /*!< Pin 0 selected */
  GPIO_STM8_API_PIN_1    = ((uint8_t)0x02),  /*!< Pin 1 selected */
  GPIO_STM8_API_PIN_2    = ((uint8_t)0x04),  /*!< Pin 2 selected */
  GPIO_STM8_API_PIN_3    = ((uint8_t)0x08),   /*!< Pin 3 selected */
  GPIO_STM8_API_PIN_4    = ((uint8_t)0x10),  /*!< Pin 4 selected */
  GPIO_STM8_API_PIN_5    = ((uint8_t)0x20),  /*!< Pin 5 selected */
  GPIO_STM8_API_PIN_6    = ((uint8_t)0x40),  /*!< Pin 6 selected */
  GPIO_STM8_API_PIN_7    = ((uint8_t)0x80)  /*!< Pin 7 selected */
};

#include "stm8s.h"

#define GPIO_STM8_API_PORT_A	GPIOA
#define GPIO_STM8_API_PORT_B	GPIOB
#define GPIO_STM8_API_PORT_C	GPIOC
#define GPIO_STM8_API_PORT_D	GPIOD


#endif
