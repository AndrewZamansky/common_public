
#ifndef _GPIO_STM32F10X_API_H_
#define _GPIO_STM32F10X_API_H_

#include "gpio_api.h"
#include "src/_gpio_stm32f10x_prerequirements_check.h"

/**********  define API  types ************/


#define GPIO_STM32F10X_API_PORT_STR   	"port"
#define GPIO_STM32F10X_API_PIN_STR   	"pin"
#define GPIO_STM32F10X_API_MODE_STR   	"mode"



typedef enum
{
	IOCTL_GPIO_STM32F10X_SET_PORT_PARAM = IOCTL_LAST_COMMON_GPIO_IOCTL+1,
	IOCTL_GPIO_STM32F10X_SET_PIN_PARAM,
	IOCTL_GPIO_STM32F10X_SET_MODE_PARAM
}GPIO_STM32F10X_API_ioctl_t;



#define	GPIO_STM32F10X_API_PORT_A	0
#define	GPIO_STM32F10X_API_PORT_B	1
#define	GPIO_STM32F10X_API_PORT_C	2
#define	GPIO_STM32F10X_API_PORT_D	3

typedef enum
{
  GPIO_STM32_API_PIN_0    = ((uint16_t)0x0001),  /*!< Pin 0 selected */
  GPIO_STM32_API_PIN_1    = ((uint16_t)0x0002),  /*!< Pin 1 selected */
  GPIO_STM32_API_PIN_2    = ((uint16_t)0x0004),  /*!< Pin 2 selected */
  GPIO_STM32_API_PIN_3    = ((uint16_t)0x0008),  /*!< Pin 3 selected */
  GPIO_STM32_API_PIN_4    = ((uint16_t)0x0010),  /*!< Pin 4 selected */
  GPIO_STM32_API_PIN_5    = ((uint16_t)0x0020),  /*!< Pin 5 selected */
  GPIO_STM32_API_PIN_6    = ((uint16_t)0x0040),  /*!< Pin 6 selected */
  GPIO_STM32_API_PIN_7    = ((uint16_t)0x0080),  /*!< Pin 7 selected */
  GPIO_STM32_API_PIN_8    = ((uint16_t)0x0100),  /*!< Pin 8 selected */
  GPIO_STM32_API_PIN_9    = ((uint16_t)0x0200),  /*!< Pin 9 selected */
  GPIO_STM32_API_PIN_10   = ((uint16_t)0x0400),  /*!< Pin 10 selected */
  GPIO_STM32_API_PIN_11   = ((uint16_t)0x0800),  /*!< Pin 11 selected */
  GPIO_STM32_API_PIN_12   = ((uint16_t)0x1000),  /*!< Pin 12 selected */
  GPIO_STM32_API_PIN_13   = ((uint16_t)0x2000),  /*!< Pin 13 selected */
  GPIO_STM32_API_PIN_14   = ((uint16_t)0x4000),  /*!< Pin 14 selected */
  GPIO_STM32_API_PIN_15   = ((uint16_t)0x8000),  /*!< Pin 15 selected */

}GPIO_STM32_API_Pin_TypeDef;

#define GPIO_STM32F10X_API_MODE_OUT_PP 0x10

/**********  define API  functions  ************/

#endif
