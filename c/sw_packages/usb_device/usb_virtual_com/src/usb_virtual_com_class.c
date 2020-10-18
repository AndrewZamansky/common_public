/*
 *
 * usb_virtual_com_class.c
 *
 */

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"
#include "errors_api.h"

#include "os_wrapper.h"
#include "dev_management_api.h"

#include "usb_virtual_com_class_api.h"
#include "usb_virtual_com_class.h"
#include "uart_api.h"
#include "usb_device_api.h"
#include "usb_device_descriptors_api.h"


#define SET_LINE_CODING             0x20
#define GET_LINE_CODING             0x21
#define SET_LINE_CONTROL            0x22


struct line_coding_t {
  uint32_t bitrate;
  uint8_t format;
  uint8_t paritytype;
  uint8_t datatype;
};


#define USB_INTERFACE_DESCRIPTOR_TYPE 0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE  0x05
#define USB_DESC_INT_SIZE  0x08

#define TO_BE_SET_AUTOMATICALLY   0
#define AUTO_SET_PCKT_SIZE_MSB   0x00
#define AUTO_SET_PCKT_SIZE_LSB   0x00

static uint8_t const interface_association_descriptor[]=
{
	/*  Interface Association Descriptor:  */
	0x08,
	0x0B ,  //    bDescriptorType
	TO_BE_SET_AUTOMATICALLY, // bFirstInterface
	0x02    , // bInterfaceCount
	0x02    , // bFunctionClass
	0x02   , // bFunctionSubClass
	0x00    , // bFunctionProtocol
	0x02    ,//  iFunction
};

static uint8_t const cdc_interface[]=
{
	/*Interface Descriptor*/
	0x09,   /* bLength: Interface Descriptor size */
	USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
	/* Interface descriptor type */
	TO_BE_SET_AUTOMATICALLY,   /* bInterfaceNumber: Number of Interface */
	0x00,   /* bAlternateSetting: Alternate setting */
	0x01,   /* bNumEndpoints: One endpoints used */
	0x02,   /* bInterfaceClass: Communication Interface Class */
	0x02,   /* bInterfaceSubClass: Abstract Control Model */
	0x01,   /* bInterfaceProtocol: Common AT commands */
	0x00,   /* iInterface: */
	/*Header Functional Descriptor*/
	0x05,   /* bLength: Endpoint Descriptor size */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x00,   /* bDescriptorSubtype: Header Func Desc */
	0x10,   /* bcdCDC: spec release number */
	0x01,
	/*Call Management Functional Descriptor*/
	0x05,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x01,   /* bDescriptorSubtype: Call Management Func Desc */
	0x00,   /* bmCapabilities: D0+D1 */
	TO_BE_SET_AUTOMATICALLY,   /* bDataInterface: 1 */
	/*ACM Functional Descriptor*/
	0x04,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
//	0x00,   /* bmCapabilities */
	0x02,   /* bmCapabilities */
	/*Union Functional Descriptor*/
	0x05,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x06,   /* bDescriptorSubtype: Union func desc */
	TO_BE_SET_AUTOMATICALLY,/* bMasterInterface: Communication class interface*/
	TO_BE_SET_AUTOMATICALLY,   /* bSlaveInterface0: Data Class Interface */

	/*Endpoint for control Descriptor*/
	0x07,   /* bLength: Endpoint Descriptor size */
	USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
	TO_BE_SET_AUTOMATICALLY,   /* bEndpointAddress: (IN) */
	0x03,   /* bmAttributes: Interrupt */
	USB_DESC_INT_SIZE,      /* wMaxPacketSize: */
	0x00,
	0xFF,   /* bInterval: */
};


static uint8_t const vcom_interface[]=
{
	    /*Data class interface descriptor*/
	    0x09,   /* bLength: Endpoint Descriptor size */
	    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
	    TO_BE_SET_AUTOMATICALLY,   /* bInterfaceNumber: Number of Interface */
	    0x00,   /* bAlternateSetting: Alternate setting */
	    0x02,   /* bNumEndpoints: Two endpoints used */
	    0x0A,   /* bInterfaceClass: CDC */
	    0x00,   /* bInterfaceSubClass: */
	    0x00,   /* bInterfaceProtocol: */
	    0x00,   /* iInterface: */

	    /*Endpoint data hostOUT Descriptor*/
	    0x07,   /* bLength: Endpoint Descriptor size */
	    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
	    TO_BE_SET_AUTOMATICALLY,   /* bEndpointAddress: (OUT) */
	    0x02,   /* bmAttributes: Bulk */
		AUTO_SET_PCKT_SIZE_LSB,             /* wMaxPacketSize: */
		AUTO_SET_PCKT_SIZE_MSB,
	    0x00,   /* bInterval: ignore for Bulk transfer */

	    /*Endpoint data hostIN Descriptor*/
	    0x07,   /* bLength: Endpoint Descriptor size */
	    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
	    TO_BE_SET_AUTOMATICALLY,   /* bEndpointAddress: (IN) */
	    0x02,   /* bmAttributes: Bulk */
		AUTO_SET_PCKT_SIZE_LSB,             /* wMaxPacketSize: */
		AUTO_SET_PCKT_SIZE_MSB,
	    0x00,    /* bInterval */
};


struct line_coding_t linecoding =
{
	115200, /* baud rate*/
	0x00,   /* stop bits-1*/
	0x00,   /* parity - none*/
	0x08    /* no. of bits 8*/
};


static void new_data_received(
		struct dev_desc_t *adev, uint8_t const *buff, size_t size)
{
	struct usb_virtual_com_class_cfg_t *cfg_hndl;
	struct dev_desc_t * callback_rx_dev ;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_virtual_com_class, adev);

	callback_rx_dev = cfg_hndl->callback_rx_dev;
	if (NULL == callback_rx_dev)
	{
		return;
	}
	DEV_CALLBACK_2_PARAMS(callback_rx_dev,
			CALLBACK_DATA_RECEIVED, (void*)buff, (void*)(size_t)size);
}




static void end_of_transmit_callback(struct dev_desc_t *adev)
{
	struct usb_virtual_com_class_cfg_t *cfg_hndl;
	struct usb_virtual_com_class_runtime_t  *runtime_hndl;
	struct dev_desc_t * callback_tx_dev ;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_virtual_com_class, adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_virtual_com_class, adev);


	callback_tx_dev = cfg_hndl->callback_tx_dev;
	if ((NULL != callback_tx_dev) && (1 == runtime_hndl->tx_on))
	{
		DEV_CALLBACK_1_PARAMS(callback_tx_dev,
							CALLBACK_TX_DONE, (void*)(runtime_hndl->sentLen));
	}

}


/**
 * usb_virtual_com_pwrite()
 *
 * return:
 */
size_t usb_virtual_com_pwrite(struct dev_desc_t *adev,
			const uint8_t *apData, size_t aLength, size_t aOffset)
{
	struct dev_desc_t *usb_hw;
	struct usb_virtual_com_class_cfg_t *cfg_hndl;
	struct set_data_to_in_endpoint_t set_data_to_in_endpoint;
	struct usb_virtual_com_class_runtime_t  *runtime_hndl;
	size_t sentLen;
	uint16_t max_data_packet_size;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_virtual_com_class, adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_virtual_com_class, adev);

	usb_hw = cfg_hndl->usb_hw;
	max_data_packet_size = cfg_hndl->max_data_packet_size;

	if (aLength > max_data_packet_size)
	{
		sentLen = max_data_packet_size;
	}
	else
	{
		sentLen = aLength;
	}

	runtime_hndl->sentLen = sentLen;
	runtime_hndl->tx_on = 1;

	set_data_to_in_endpoint.endpoint_num = runtime_hndl->in_endpoint_num;
	set_data_to_in_endpoint.data = apData;
	set_data_to_in_endpoint.size = sentLen;
	DEV_IOCTL_1_PARAMS(usb_hw,
		IOCTL_USB_DEVICE_SENT_DATA_TO_IN_ENDPOINT, &set_data_to_in_endpoint);

	return sentLen;
}


/* uac_class_in_request()
 *
 */
static void cdc_class_in_request( struct dev_desc_t *usb_hw, uint8_t *request)
{
	struct set_request_in_buffer_t set_request_in_buffer;
	uint8_t ret;

	ret = 0;
	// Device to host
	switch(request[1])
	{
	case GET_LINE_CODING:
		set_request_in_buffer.data = (uint8_t*)&linecoding;
		set_request_in_buffer.size = 7;
		break;
	default:
		ret = 1;
		break;
	}

	if (0 == ret)
	{
		DEV_IOCTL_1_PARAMS(usb_hw,
				IOCTL_USB_DEVICE_SET_REQUEST_IN_BUFFER, &set_request_in_buffer);
	}
	else
	{
		/* Setup error, stall the device */
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_STALL);
	}
}



/*  uac_class_out_request()
 *
 */
static void cdc_class_out_request( struct dev_desc_t *usb_hw, uint8_t *request)
{
	uint8_t ret;
	struct set_request_out_buffer_t set_request_out_buffer;

	ret = 0;
    switch(request[1])
	{
	case SET_LINE_CODING:
		set_request_out_buffer.data = (uint8_t *)&linecoding;
		set_request_out_buffer.size = 7;
		break;
	case SET_LINE_CONTROL:
		set_request_out_buffer.data = NULL;
		set_request_out_buffer.size = 0;
		break;
	default:
		ret = 1;
		break;
	}

    if (0 == ret)
    {
		DEV_IOCTL_1_PARAMS(usb_hw,
			IOCTL_USB_DEVICE_SET_REQUEST_OUT_BUFFER, &set_request_out_buffer);
    }
	else
	{
		/* Setup error, stall the device */
		DEV_IOCTL_0_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_STALL);
	}
}


static void vcom_class_request(
		struct dev_desc_t   *callback_dev, uint8_t *request)
{
	struct usb_virtual_com_class_cfg_t *cfg_hndl;
	struct dev_desc_t *usb_hw;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_virtual_com_class, callback_dev);
	usb_hw = cfg_hndl->usb_hw;

	if(request[0] & 0x80)    /* request data transfer direction */
	{// from device to host
		cdc_class_in_request(usb_hw, request);
	}
	else
	{// from host to device
		cdc_class_out_request(usb_hw, request);
	}
}


static void configure_endpoints(struct dev_desc_t *adev,
		struct usb_virtual_com_class_cfg_t *cfg_hndl,
		uint8_t *i_cdc, uint8_t *i_vcom)
{
	struct usb_virtual_com_class_runtime_t *runtime_hndl;
	struct set_endpoints_t set_endpoints;
	struct dev_desc_t *usb_hw;
	usb_dev_in_endpoint_callback_func_t   in_func_arr[3];
	uint8_t   endpoints_num_arr[3];
	usb_dev_out_endpoint_callback_func_t   out_func_arr[3];
	usb_dev_endpoint_request_callback_func_t  endpoint_request_callback_func[3];
	uint8_t    endpoints_type_arr[3];
	uint16_t   max_pckt_sizes[3];
	uint16_t max_data_packet_size;

	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_virtual_com_class, adev);
	usb_hw = cfg_hndl->usb_hw;
	max_data_packet_size = cfg_hndl->max_data_packet_size;

	set_endpoints.num_of_endpoints = 3;
	set_endpoints.endpoints_num_arr = endpoints_num_arr;
	endpoint_request_callback_func[0] = NULL;
	endpoint_request_callback_func[1] = NULL;
	endpoint_request_callback_func[2] = NULL;
	out_func_arr[0] = NULL;
	out_func_arr[1] = new_data_received;
	out_func_arr[2] = NULL;
	in_func_arr[0] = NULL;
	in_func_arr[1] = NULL;
	in_func_arr[2] = end_of_transmit_callback;
	set_endpoints.out_func_arr = out_func_arr;
	set_endpoints.in_func_arr = in_func_arr;
	set_endpoints.endpoint_request_callback_func =
							endpoint_request_callback_func;
	set_endpoints.callback_dev = adev;
	max_pckt_sizes[0] = USB_DESC_INT_SIZE;
	max_pckt_sizes[1] = max_data_packet_size;
	max_pckt_sizes[2] = max_data_packet_size;
	set_endpoints.max_pckt_sizes = max_pckt_sizes;
	endpoints_type_arr[0] = USB_DEVICE_API_EP_TYPE_INTERRUPT_IN;
	endpoints_type_arr[1] = USB_DEVICE_API_EP_TYPE_BULK_OUT;
	endpoints_type_arr[2] = USB_DEVICE_API_EP_TYPE_BULK_IN;
	set_endpoints.endpoints_type_arr = endpoints_type_arr;
	DEV_IOCTL_1_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_ENDPOINTS, &set_endpoints);

	i_cdc[28 + 2] = 0x80 | endpoints_num_arr[0];// 0x80 for IN endpoint

	i_vcom[9 + 2] = endpoints_num_arr[1];
	i_vcom[9 + 4] = max_data_packet_size & 0xFF;
	i_vcom[9 + 5] = (max_data_packet_size >> 8) & 0xFF;

	i_vcom[16 + 2] = 0x80 | endpoints_num_arr[2];
	i_vcom[16 + 4] = max_data_packet_size & 0xFF;
	i_vcom[16 + 5] = (max_data_packet_size >> 8) & 0xFF;
	runtime_hndl->in_endpoint_num = endpoints_num_arr[2];
}


static void update_configuration_desc(struct dev_desc_t *adev,
	struct usb_virtual_com_class_cfg_t *cfg_hndl,
	struct usb_descriptors_alloc_interfaces_t *usb_descriptors_alloc_interfaces)
{
	struct dev_desc_t *usb_descriptors_dev;
	struct usb_descriptors_add_interface_t usb_desc_add_interface;
	uint8_t *iad;
	uint8_t *i_cdc;
	uint8_t *i_vcom;

	usb_descriptors_dev = cfg_hndl->usb_descriptors_dev;

	iad = (uint8_t*)os_safe_malloc(sizeof(interface_association_descriptor)) ;
	errors_api_check_if_malloc_succeed(iad);
	memcpy(iad, interface_association_descriptor,
						sizeof(interface_association_descriptor));
	iad[2] = usb_descriptors_alloc_interfaces->interfaces_num[0];
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE_ASSOCIATION_DESCRIPTOR, iad);
	os_safe_free(iad);

	i_cdc = (uint8_t*)os_safe_malloc(sizeof(cdc_interface)) ;
	errors_api_check_if_malloc_succeed(i_cdc);
	memcpy(i_cdc, cdc_interface, sizeof(cdc_interface));
	i_vcom = (uint8_t*)os_safe_malloc(sizeof(vcom_interface)) ;
	errors_api_check_if_malloc_succeed(i_vcom);
	memcpy(i_vcom, vcom_interface, sizeof(vcom_interface));

	i_cdc[2] = usb_descriptors_alloc_interfaces->interfaces_num[0];
	i_cdc[14 + 4] = usb_descriptors_alloc_interfaces->interfaces_num[1];
	i_cdc[23 + 3] = usb_descriptors_alloc_interfaces->interfaces_num[0];
	i_cdc[23 + 4] = usb_descriptors_alloc_interfaces->interfaces_num[1];
	i_vcom[2] = usb_descriptors_alloc_interfaces->interfaces_num[1];

	configure_endpoints(adev, cfg_hndl, i_cdc, i_vcom);

	usb_desc_add_interface.interface_desc = i_cdc;
	usb_desc_add_interface.interface_desc_size = sizeof(cdc_interface);
	usb_desc_add_interface.alt_interface_desc = NULL;
	usb_desc_add_interface.is_hid_interface = 0;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);


	usb_desc_add_interface.interface_desc = i_vcom;
	usb_desc_add_interface.interface_desc_size = sizeof(vcom_interface);
	usb_desc_add_interface.alt_interface_desc = NULL;
	usb_desc_add_interface.is_hid_interface = 0;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_add_interface);
	os_safe_free(i_cdc);
	os_safe_free(i_vcom);
}


static void start_virtual_com_class(struct dev_desc_t *adev,
		struct usb_virtual_com_class_cfg_t *cfg_hndl,
		struct usb_virtual_com_class_runtime_t *runtime_hndl)
{
	struct usb_descriptors_alloc_interfaces_t usb_descriptors_alloc_interfaces;
	struct dev_desc_t *usb_descriptors_dev;
	struct dev_desc_t *usb_hw;
	struct register_interfaces_t register_interfaces;
	struct register_interface_t register_interface[2];


	if (1 == runtime_hndl->init_done)
	{
		return;
	}
	runtime_hndl->init_done = 1;

	usb_descriptors_dev = cfg_hndl->usb_descriptors_dev;
	usb_hw = cfg_hndl->usb_hw;
	usb_descriptors_alloc_interfaces.num_of_interfaces = 2;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ALLOC_INTERFACES_NUMBERS,
			&usb_descriptors_alloc_interfaces);


	update_configuration_desc(
			adev, cfg_hndl, &usb_descriptors_alloc_interfaces);

	register_interface[0].interfaces_num =
			usb_descriptors_alloc_interfaces.interfaces_num[0];
	register_interface[0].interface_func = vcom_class_request;
	register_interface[1].interfaces_num =
			usb_descriptors_alloc_interfaces.interfaces_num[1];
	register_interface[1].interface_func = vcom_class_request;

	register_interfaces.num_of_interfaces = 2;
	register_interfaces.register_interface_arr = register_interface;
	register_interfaces.callback_dev = adev;
	DEV_IOCTL_1_PARAMS(usb_hw,
			IOCTL_USB_DEVICE_REGISTER_INTERFACES, &register_interfaces);
}


/**
 * usb_virtual_com_class_ioctl()
 *
 * return:
 */
static uint8_t usb_virtual_com_class_ioctl(struct dev_desc_t *adev,
		uint8_t aIoctl_num, void * aIoctl_param1, void * aIoctl_param2)
{
	struct usb_virtual_com_class_cfg_t *cfg_hndl;
	struct usb_virtual_com_class_runtime_t *runtime_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(usb_virtual_com_class, adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(usb_virtual_com_class, adev);

	switch(aIoctl_num)
	{
	case IOCTL_DEVICE_START :
		break;
	case IOCTL_USB_VIRTUAL_COM_START :
		start_virtual_com_class(adev, cfg_hndl, runtime_hndl);
		break;
	case IOCTL_UART_DISABLE_TX :
		runtime_hndl->tx_on = 0;
		break;
	default :
		return 1;
	}
	return 0;
}

#define    MODULE_NAME                  usb_virtual_com_class
#define    MODULE_IOCTL_FUNCTION        usb_virtual_com_class_ioctl
#define    MODULE_PWRITE_FUNCTION       usb_virtual_com_pwrite
#include "add_module.h"
