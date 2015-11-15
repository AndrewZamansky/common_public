
#ifndef _HTTP_API_H_
#define _HTTP_API_H_

/**********  define API  types ************/

//typedef uint8_t (*OpenLowLevelSocket_func_t)(uint8_t *ManagmentServerIP, uint8_t *port ,void **pSocket)  ;
//
//typedef uint8_t (*CloseLowLevelSocket_func_t)(const void *socketHandle)  ;
//
//
//typedef struct {
//	uint8_t *ManagmentServerIP;
//	uint8_t *ManagmentServerPort;
//	pdev_descriptor  server_device;
////	OpenLowLevelSocket_func_t fOpenLowLevelSocket;
////	CloseLowLevelSocket_func_t   fCloseLowLevelSocket;
////	pTransferData_func_t    fSendDataToLowLevelSocket;
//	pCommNotifyForNewData_func_t  fNotifyForNewData;
//} HTTP_API_Init_params_t;

#define HTTP_API_MNG_SERVER_IP_STR   			"managment_server_ip"
#define HTTP_API_MNG_SERVER_PORT_STR   			"managment_server_port"
#define HTTP_API_MNG_SERVER_IP_REDANDENCY_STR   "managment_server_ip_redandancy"
#define HTTP_API_MNG_SERVER_PORT_REDANDENCY_STR "managment_server_port_redandancy"
#define HTTP_API_SERVER_DEVICE_STR  			"server_device"

typedef enum
{
	IOCTL_HTTP_SET_MNG_SERVER_IP = IOCTL_LAST_COMMON_IOCTL+1,
	IOCTL_HTTP_SET_MNG_SERVER_PORT,
	IOCTL_HTTP_SET_MNG_SERVER_IP_REDANDENCY,
	IOCTL_HTTP_SET_MNG_SERVER_PORT_REDANDENCY,
	IOCTL_HTTP_SET_SERVER_DEVICE,
}HTTP_API_ioctl_t;


/**********  define API  functions  ************/

uint8_t  http_server_api_init_dev_descriptor(pdev_descriptor aDevDescriptor);


#else
#pragma message( "warning : this header file had already been included" )
#endif
