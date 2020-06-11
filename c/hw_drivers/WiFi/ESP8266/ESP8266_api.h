
#ifndef _ESP8266_API_H_
#define _ESP8266_API_H_

#ifdef  __cplusplus
extern "C" {
#endif


#define ESP8266_API_CHIP_TYPE_STR  				"chip"
#define ESP8266_API_SSID_NAME_STR   			"ssid_name"
#define ESP8266_API_SSID_PSWRD_STR   			"ssid_pswrd"
#define ESP8266_API_SSID_NAME_REDANDENCY_STR   	"ssid_name_redandancy"
#define ESP8266_API_SSID_PSWRD_REDANDENCY_STR  	"ssid_pswrd_redandancy"
#define ESP8266_API_SERVER_DEVICE_STR  			"server_device"

enum ESP8266_err_code_e {
	ESP8266_NO_ERR = 0,
	ESP8266_ERR_UNKOWN,
	ESP8266_ERR_SOCKET_NOT_AVAILABLE,
	ESP8266_ERR_NO_FREE_SOCKET_LEFT,
	ESP8266_ERR_REQUEST_TIMED_OUT,
	ESP8266_ERR_CANNOT_SEND_BAD_LINK,
	ESP8266_ERR_CANNOT_CONNECT,
	ESP8266_ERR_CANNOT_CONNECT_LINK_ERR,
	ESP8266_ERR_CANNOT_CLOSE
};

enum ESP8266_API_ioctl_e {
	IOCTL_ESP8266_SET_CHIP_TYPE = IOCTL_LAST_COMMON_IOCTL + 1,
	IOCTL_ESP8266_SET_SSID_NAME,
	IOCTL_ESP8266_SET_SSID_PSWRD,
	IOCTL_ESP8266_SET_SSID_NAME_REDANDENCY,
	IOCTL_ESP8266_SET_SSID_PSWRD_REDANDENCY,
	IOCTL_ESP8266_SET_SERVER_DEVICE,
	IOCTL_ESP8266_SOCKET_OPEN
};


struct ESP8266_ioctl_socket_open_t {
	void ** new_socket_descriptor;
};



struct esp8266_cfg_t {
	struct dev_desc_t *  timer_dev;
	struct dev_desc_t *  uart_rx_dev;
	struct dev_desc_t *  uart_tx_dev;
	struct dev_desc_t *  uart_dev;
};
SET_CONFIG_TYPE(ESP8266, struct esp8266_cfg_t);


#ifdef  __cplusplus
}
#endif

#endif
