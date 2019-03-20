
#ifndef _ESP8266_API_H_
#define _ESP8266_API_H_

/*****************  defines  **************/


/**********  define API  types ************/

#ifdef  __cplusplus
extern "C" {
#endif

typedef void*  ESP8266_API_Handle_t;



#define ESP8266_API_CHIP_TYPE_STR  				"chip"
#define ESP8266_API_SSID_NAME_STR   			"ssid_name"
#define ESP8266_API_SSID_PSWRD_STR   			"ssid_pswrd"
#define ESP8266_API_SSID_NAME_REDANDENCY_STR   	"ssid_name_redandancy"
#define ESP8266_API_SSID_PSWRD_REDANDENCY_STR  	"ssid_pswrd_redandancy"
#define ESP8266_API_SERVER_DEVICE_STR  			"server_device"

typedef enum
{
	ESP8266_NO_ERR = 0,
	ESP8266_ERR_UNKOWN,
	ESP8266_ERR_SOCKET_NOT_AVAILABLE
}ESP8266_err_code_t;

typedef enum
{
	IOCTL_ESP8266_SET_CHIP_TYPE = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_ESP8266_SET_SSID_NAME,
	IOCTL_ESP8266_SET_SSID_PSWRD,
	IOCTL_ESP8266_SET_SSID_NAME_REDANDENCY,
	IOCTL_ESP8266_SET_SSID_PSWRD_REDANDENCY,
	IOCTL_ESP8266_SET_SERVER_DEVICE,
	IOCTL_ESP8266_SOCKET_CLOSE,
	IOCTL_ESP8266_SOCKET_OPEN,
	IOCTL_ESP8266_SOCKET_CONNECT,
	IOCTL_ESP8266_SOCKET_IS_DATA_RECEIVED,
	IOCTL_ESP8266_SOCKET_GET_RECEIVED_DATA,
	IOCTL_ESP8266_SOCKET_GET_OPEN_CONNECTION_STATUS,
	IOCTL_ESP8266_SOCKET_SET_OPTIONS,
	IOCTL_ESP8266_SOCKET_GET_OPTIONS
}ESP8266_API_ioctl_t;


struct ESP8266_ioctl_socket_open_t {
	struct dev_desc_t **  new_socket_descriptor;
};


struct ESP8266_ioctl_socket_connect_t {
	const char *strHostName;
	char *strPort;
};


struct ESP8266_ioctl_get_conn_status_t {
	char *strIP;
	uint8_t strIPLen;
	uint16_t* pPort;
};


struct ESP8266_ioctl_data_received_t {
	uint8_t *buffer;
	size_t max_size;
	size_t *size_received;
};

/**********  define API  functions  ************/


#ifdef  __cplusplus
}
#endif

#endif
