/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "esp8266_api.h"
#include 	"src/esp8266.h"

uint8_t ESP8266_callback(pdev_descriptor_t apdev , const uint8_t aCallback_num , void * aCallback_param1, void * aCallback_param2);
uint8_t ESP8266_ioctl( pdev_descriptor_t apdev ,const uint8_t aIoctl_num , void * aIoctl_param1 , void * aIoctl_param2);

#define	MODULE_NAME					esp8266
#define	MODULE_IOCTL_FUNCTION		ESP8266_ioctl
#define	MODULE_CALLBACK_FUNCTION	ESP8266_callback
#define MODULE_CONFIG_DATA_STRUCT_TYPE		esp8266_instance_t

#ifdef DT_DEV_MODULE

	#ifdef ESP8266_DT_TIMER_PDEV
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(ESP8266_DT_TIMER_PDEV) ;
		#define POINTER_TO_TIMER_PDEV		P_TO_STATIC_DEVICE_INST(ESP8266_DT_TIMER_PDEV)
	#else
		#error "ESP8266_DT_TIMER_PDEV should be defined"
	#endif

	#ifdef ESP8266_DT_UART_TX_DEV
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(ESP8266_DT_UART_TX_DEV) ;
		#define POINTER_TO_UART_TX_PDEV		P_TO_STATIC_DEVICE_INST(ESP8266_DT_UART_TX_DEV)
	#else
		#error "ESP8266_DT_UART_TX_DEV should be defined"
	#endif

	#ifdef ESP8266_DT_UART_RX_DEV
		EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(ESP8266_DT_UART_RX_DEV) ;
		#define POINTER_TO_UART_RX_PDEV		P_TO_STATIC_DEVICE_INST(ESP8266_DT_UART_RX_DEV)
	#else
		#error "ESP8266_UART_RX_DEV should be defined"
	#endif


	#define STATIC_DEV_DATA_STRUCT			\
		{									\
			POINTER_TO_TIMER_PDEV ,			\
			POINTER_TO_UART_RX_PDEV ,		\
			POINTER_TO_UART_TX_PDEV ,		\
		}

	#endif

#include "add_component.h"

/* device specific defines should be undefined after calling #include "add_static_dev.h" */
#undef POINTER_TO_UART_RX_PDEV
#undef POINTER_TO_UART_TX_PDEV
#undef POINTER_TO_TIMER_PDEV
#undef ESP8266_DT_TIMER_PDEV
#undef ESP8266_DT_UART_TX_DEV
#undef ESP8266_DT_UART_RX_DEV
