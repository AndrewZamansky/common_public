
#ifndef _HTTP_API_H_
#define _HTTP_API_H_


#define HTTP_API_MNG_SERVER_IP_STR   			"management_server_ip"
#define HTTP_API_MNG_SERVER_PORT_STR   			"management_server_port"
#define HTTP_API_MNG_SERVER_IP_REDANDENCY_STR   "management_server_ip_redandancy"
#define HTTP_API_MNG_SERVER_PORT_REDANDENCY_STR "management_server_port_redandancy"
#define HTTP_API_SERVER_DEVICE_STR  			"server_device"

enum HTTP_API_ioctl_e {
	IOCTL_HTTP_SET_MNG_SERVER_IP = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_HTTP_SET_MNG_SERVER_PORT,
	IOCTL_HTTP_SET_MNG_SERVER_IP_REDANDENCY,
	IOCTL_HTTP_SET_MNG_SERVER_PORT_REDANDENCY,
	IOCTL_HTTP_SET_SERVER_DEVICE,
	IOCTL_HTTP_SET_TIMER_DEVICE,
};



uint8_t  http_server_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor);

#endif
