
#ifndef _GPIO_NUC505_API_H_
#define _GPIO_NUC505_API_H_


#include "gpio_api.h"



#define GPIO_NUC505_API_PORT_STR   			"port"
#define GPIO_NUC505_API_SINGLE_PIN_STR   	"single_pin"
#define GPIO_NUC505_API_MODE_STR   			"mode"

enum GPIO_NUC505_API_Pin_e {
  GPIO_NUC505_API_PIN_0    = ((uint8_t)0x01),  /*!< Pin 0 selected */
  GPIO_NUC505_API_PIN_1    = ((uint8_t)0x02),  /*!< Pin 1 selected */
  GPIO_NUC505_API_PIN_2    = ((uint8_t)0x04),  /*!< Pin 2 selected */
  GPIO_NUC505_API_PIN_3    = ((uint8_t)0x08),   /*!< Pin 3 selected */
  GPIO_NUC505_API_PIN_4    = ((uint8_t)0x10),  /*!< Pin 4 selected */
  GPIO_NUC505_API_PIN_5    = ((uint8_t)0x20),  /*!< Pin 5 selected */
  GPIO_NUC505_API_PIN_6    = ((uint8_t)0x40),  /*!< Pin 6 selected */
  GPIO_NUC505_API_PIN_7    = ((uint8_t)0x80)  /*!< Pin 7 selected */
};

#include "gpio.h"

#define GPIO_NUC505_API_PORT_A		GPIOA_BASE
#define GPIO_NUC505_API_PORT_B		GPIOB_BASE
#define GPIO_NUC505_API_PORT_C		GPIOC_BASE
#define GPIO_NUC505_API_PORT_D		GPIOD_BASE

enum GPIO_NUC505_API_ioctl_e {
	IOCTL_GPIO_NUC505_SET_PORT_PARAM = IOCTL_LAST_COMMON_GPIO_IOCTL+1,
	IOCTL_GPIO_NUC505_SET_SINGLE_PIN_NUMBER_PARAM,
	IOCTL_GPIO_NUC505_SET_MODE_PARAM
};


#define GPIO_NUC505_API_MODE_OUT_PP 	GPIO_MODE_OUTPUT
#define GPIO_NUC505_API_MODE_IN		 	GPIO_MODE_INPUT

#endif
