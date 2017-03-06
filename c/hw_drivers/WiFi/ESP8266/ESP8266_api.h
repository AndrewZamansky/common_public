
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
	IOCTL_ESP8266_SOCKET_CONNECT,
	IOCTL_ESP8266_SOCKET_IS_DATA_RECEIVED,
	IOCTL_ESP8266_SOCKET_GET_RECEIVED_DATA,
	IOCTL_ESP8266_SOCKET_GET_OPEN_CONNECTION_STATUS
}ESP8266_API_ioctl_t;

typedef struct
{
	pdev_descriptor_t*  new_socket_descriptor;
}ESP8266_ioctl_socket_open_t;

typedef struct
{
	uint8_t *strHostName;
	uint8_t *strPort;
}ESP8266_ioctl_socket_connect_t;

typedef struct
{
	uint8_t *strIP;
	uint8_t strIPLen;
	uint16_t* pPort;
}ESP8266_ioctl_socket_get_open_connection_t;

typedef struct
{
	uint8_t *strIP;
	uint8_t strIPLen;
}ESP8266_ioctl_socket_get_ip_t;

typedef struct
{
	uint8_t *buffer;
	size_t max_size;
	size_t *size_received;
}ESP8266_ioctl_data_received_t;

/**********  define API  functions  ************/

uint8_t ESP8266_api_init_dev_descriptor(pdev_descriptor_t aDevDescriptor);
void set_esp8266_pdev_for_socket_api(pdev_descriptor_t a_esp8266_dev);

#endif
