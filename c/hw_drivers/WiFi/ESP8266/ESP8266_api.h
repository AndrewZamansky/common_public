
#ifndef _ESP8266_API_H_
#define _ESP8266_API_H_

/*****************  defines  **************/


/**********  define API  types ************/

typedef void*  ESP8266_API_Handle_t;



#define ESP8266_API_CHIP_TYPE_STR  				"chip"
#define ESP8266_API_SSID_NAME_STR   			"ssid_name"
#define ESP8266_API_SSID_PSWRD_STR   			"ssid_pswrd"
#define ESP8266_API_SSID_NAME_REDANDENCY_STR   	"ssid_name_redandancy"
#define ESP8266_API_SSID_PSWRD_REDANDENCY_STR  	"ssid_pswrd_redandancy"
#define ESP8266_API_SERVER_DEVICE_STR  			"server_device"

typedef enum
{
	IOCTL_ESP8266_SET_CHIP_TYPE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_ESP8266_SET_SSID_NAME,
	IOCTL_ESP8266_SET_SSID_PSWRD,
	IOCTL_ESP8266_SET_SSID_NAME_REDANDENCY,
	IOCTL_ESP8266_SET_SSID_PSWRD_REDANDENCY,
	IOCTL_ESP8266_SET_SERVER_DEVICE,
	IOCTL_ESP8266_GET_IP,
	IOCTL_ESP8266_SOCKET_CLOSE,
	IOCTL_ESP8266_SOCKET_OPEN,
}ESP8266_API_ioctl_t;

typedef struct
{
	uint8_t *strIP;
	uint8_t *strPort;
	pdev_descriptor*  new_socket_descriptor;
}ioctl_socket_open_t;


/**********  define API  functions  ************/

uint8_t ESP8266_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);

#else
#pragma message( "warning : this header file had already been included" )
#endif
