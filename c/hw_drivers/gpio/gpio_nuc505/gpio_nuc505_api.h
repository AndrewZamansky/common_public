
#ifndef _GPIO_NUC505_API_H_
#define _GPIO_NUC505_API_H_

#include "gpio_NUC505_config.h"
#include "dev_managment_api.h" // for device manager defines and typedefs
#include "gpio_api.h"
#include "src/_gpio_NUC505_prerequirements_check.h" // should be after {uart_NUC505_config.h,dev_managment_api.h}

/**********  define API  types ************/


#define GPIO_NUC505_API_PORT_STR   	"port"
#define GPIO_NUC505_API_PIN_STR   	"pin"
#define GPIO_NUC505_API_MODE_STR   	"mode"

typedef enum
{
  GPIO_NUC505_API_PIN_0    = ((uint8_t)0x01),  /*!< Pin 0 selected */
  GPIO_NUC505_API_PIN_1    = ((uint8_t)0x02),  /*!< Pin 1 selected */
  GPIO_NUC505_API_PIN_2    = ((uint8_t)0x04),  /*!< Pin 2 selected */
  GPIO_NUC505_API_PIN_3    = ((uint8_t)0x08),   /*!< Pin 3 selected */
  GPIO_NUC505_API_PIN_4    = ((uint8_t)0x10),  /*!< Pin 4 selected */
  GPIO_NUC505_API_PIN_5    = ((uint8_t)0x20),  /*!< Pin 5 selected */
  GPIO_NUC505_API_PIN_6    = ((uint8_t)0x40),  /*!< Pin 6 selected */
  GPIO_NUC505_API_PIN_7    = ((uint8_t)0x80)  /*!< Pin 7 selected */
}GPIO_NUC505_API_Pin_TypeDef;

#include "gpio.h"

#define GPIO_NUC505_API_PORT_A	PA
#define GPIO_NUC505_API_PORT_B	PB
#define GPIO_NUC505_API_PORT_C	PC
#define GPIO_NUC505_API_PORT_D	PD

typedef enum
{
	IOCTL_GPIO_NUC505_SET_PORT_PARAM = IOCTL_LAST_COMMON_GPIO_IOCTL+1,
	IOCTL_GPIO_NUC505_SET_PIN_PARAM,
	IOCTL_GPIO_NUC505_SET_MODE_PARAM
}GPIO_NUC505_API_ioctl_t;


#define GPIO_NUC505_API_MODE_OUT_PP 	GPIO_MODE_OUTPUT


/**********  define API  functions  ************/




uint8_t  gpio_NUC505_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);


#include "src/_gpio_NUC505_static_dev_macros.h"

#define GPIO_NUC505_API_CREATE_STATIC_DEV(dev,dev_name ,port , pin , mode) \
		__GPIO_NUC505_API_CREATE_STATIC_DEV(dev,dev_name ,port , pin , mode)

#endif
