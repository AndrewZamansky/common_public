/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "gpio_pic32mx575_api.h"
#include "src/gpio_pic32mx575.h"

uint8_t gpio_pic32mx575_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME				gpio_pic32mx575
#define	MODULE_IOCTL_FUNCTION	gpio_pic32mx575_ioctl
#define MODULE_CONFIG_DATA_STRUCT_TYPE	gpio_pic32mx575_instance_t

#ifdef DT_DEV_MODULE

	#ifndef GPIO_PIC32MX575_DT_PACKAGE_PIN_NUMBER
		#error "GPIO_PIC32MX575_DT_PACKAGE_PIN_NUMBER should be defined"
	#endif

	#ifndef GPIO_PIC32MX575_DT_PORT
		#error "GPIO_PIC32MX575_DT_PORT should be defined"
	#endif

	#ifndef GPIO_PIC32MX575_DT_PIN
		#error "GPIO_PIC32MX575_DT_PIN should be defined"
	#endif


	#ifndef GPIO_PIC32MX575_DT_MODE
		#error "GPIO_PIC32MX575_DT_MODE should be defined"
	#endif



	#define STATIC_DEV_DATA_STRUCT	\
		{							\
			GPIO_PIC32MX575_DT_PORT,	\
			GPIO_PIC32MX575_DT_PIN,		\
			GPIO_PIC32MX575_DT_PACKAGE_PIN_NUMBER ,	\
			GPIO_PIC32MX575_DT_MODE	\
		}

#endif


#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_component.h" */
#undef GPIO_PIC32MX575_DT_PACKAGE_PIN_NUMBER
#undef GPIO_PIC32MX575_DT_MODE
