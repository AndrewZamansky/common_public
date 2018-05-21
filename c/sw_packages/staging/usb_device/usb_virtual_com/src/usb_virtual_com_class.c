/*
 *
 * usb_virtual_com_stm32f103x.c
 *
 *
 *
 *
 *
 */

/********  includes *********************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h" // for device manager defines and typedefs

#include "usb_virtual_com_class.h"
#include "uart_api.h"

/********  defines *********************/


/********  types  *********************/


/* ---------------------------- External variables ---------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------ Exported variables ------------------------------*/



typedef struct
{
  uint32_t bitrate;
  uint8_t format;
  uint8_t paritytype;
  uint8_t datatype;
}LINE_CODING;


/********  externals *********************/


/********  local defs *********************/

static struct dev_desc_t *   callback_dev;

static uint32_t sentLen;


static uint8_t USB_Rx_Buffer[USB_MAX_DATA_SIZE];

#define USB_INTERFACE_DESCRIPTOR_TYPE 0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE  0x05
#define USB_DESC_INT_SIZE  0x08
#define USB_MAX_DATA_SIZE  64

static const uint8_t cdc_interface[]=
{
	/*Interface Descriptor*/
	0x09,   /* bLength: Interface Descriptor size */
	USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
	/* Interface descriptor type */
	0x00,   /* bInterfaceNumber: Number of Interface */
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
	0x01,   /* bDataInterface: 1 */
	/*ACM Functional Descriptor*/
	0x04,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
	0x02,   /* bmCapabilities */
	/*Union Functional Descriptor*/
	0x05,   /* bFunctionLength */
	0x24,   /* bDescriptorType: CS_INTERFACE */
	0x06,   /* bDescriptorSubtype: Union func desc */
	0x00,   /* bMasterInterface: Communication class interface */
	0x01,   /* bSlaveInterface0: Data Class Interface */
	/*Endpoint 2 Descriptor*/
	0x07,   /* bLength: Endpoint Descriptor size */
	USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
	0x82,   /* bEndpointAddress: (IN2) */
	0x03,   /* bmAttributes: Interrupt */
	USB_DESC_INT_SIZE,      /* wMaxPacketSize: */
	0x00,
	0xFF,   /* bInterval: */
};


static const uint8_t vcom_interface[]=
{
	    /*Data class interface descriptor*/
	    0x09,   /* bLength: Endpoint Descriptor size */
	    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
	    0x01,   /* bInterfaceNumber: Number of Interface */
	    0x00,   /* bAlternateSetting: Alternate setting */
	    0x02,   /* bNumEndpoints: Two endpoints used */
	    0x0A,   /* bInterfaceClass: CDC */
	    0x00,   /* bInterfaceSubClass: */
	    0x00,   /* bInterfaceProtocol: */
	    0x00,   /* iInterface: */
	    /*Endpoint 3 Descriptor*/
	    0x07,   /* bLength: Endpoint Descriptor size */
	    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
	    0x03,   /* bEndpointAddress: (OUT3) */
	    0x02,   /* bmAttributes: Bulk */
	    USB_MAX_DATA_SIZE,             /* wMaxPacketSize: */
	    0x00,
	    0x00,   /* bInterval: ignore for Bulk transfer */
	    /*Endpoint 1 Descriptor*/
	    0x07,   /* bLength: Endpoint Descriptor size */
	    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
	    0x81,   /* bEndpointAddress: (IN1) */
	    0x02,   /* bmAttributes: Bulk */
	    USB_MAX_DATA_SIZE,             /* wMaxPacketSize: */
	    0x00,
	    0x00,    /* bInterval */
}


LINE_CODING linecoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* no. of bits 8*/
  };


/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback (void)
{
//	DEV_CALLBACK_1_PARAMS(callback_dev , CALLBACK_TX_DONE, (void*)sentLen);
}

static void new_data_received(
		struct dev_desc_t *adev, uint8_t const *buff, size_t size)
{
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
//	uint32_t USB_Rx_Cnt;
//	/* Get the received data buffer and update the counter */
//	USB_Rx_Cnt = USB_SIL_Read(EP3_OUT, USB_Rx_Buffer);
//	DEV_CALLBACK_2_PARAMS(callback_dev , CALLBACK_DATA_RECEIVED, USB_Rx_Buffer, (void*)USB_Rx_Cnt);
//
//#ifndef STM32F10X_CL
//  /* Enable the receive of data on EP3 */
//  SetEPRxValid(ENDP3);
//#endif /* STM32F10X_CL */
}





/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetLineCoding.
* Description    : send the linecoding structure to the PC host.
* Input          : Length.
* Output         : None.
* Return         : Linecoding structure base address.
*******************************************************************************/
uint8_t *Virtual_Com_Port_GetLineCoding(uint16_t Length)
{
//  if (Length == 0)
//  {
//    pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
//    return NULL;
//  }
//  return(uint8_t *)&linecoding;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_SetLineCoding.
* Description    : Set the linecoding structure fields.
* Input          : Length.
* Output         : None.
* Return         : Linecoding structure base address.
*******************************************************************************/
uint8_t *Virtual_Com_Port_SetLineCoding(uint16_t Length)
{
//  if (Length == 0)
//  {
//    pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
//    return NULL;
//  }
//  return(uint8_t *)&linecoding;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        usb_virtual_com_pwrite                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
size_t usb_virtual_com_pwrite(const void *aHandle ,const uint8_t *apData , size_t aLength, size_t aOffset)
{
//    if (aLength > USB_MAX_DATA_SIZE)
//    {
//    	sentLen = USB_MAX_DATA_SIZE;
//    }
//    else
//    {
//    	sentLen = aLength;
//    }
//
//#ifdef USE_STM3210C_EVAL
//    USB_SIL_Write(EP1_IN,apData, lCurrent_pos);
//#else
//    UserToPMABufferCopy(( uint8_t *)apData, ENDP1_TXADDR, sentLen);
//    SetEPTxCount(ENDP1, sentLen);
//    SetEPTxValid(ENDP1);
//#endif
//
//    return sentLen;
}




static void start_audio_class(struct dev_desc_t *adev,
		struct usb_audio_class_cfg_t *cfg_hndl,
		struct usb_audio_class_runtime_t *runtime_hndl)
{
	struct set_out_endpoint_callback_t set_out_endpoint_callback;
	struct usb_descriptors_set_interface_t usb_desc_set_interface;
	struct dev_desc_t *usb_hw;
	struct dev_desc_t *usb_descriptors_dev;
	uint32_t buff_size;
	uint8_t i;

	usb_descriptors_dev = cfg_hndl->usb_descriptors_dev;

	usb_desc_set_interface.interface_desc = cdc_interface;
	usb_desc_set_interface.interface_desc_size = sizeof(cdc_interface);
	usb_desc_set_interface.alt_interface_desc = NULL;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_set_interface);


	usb_desc_set_interface.interface_desc = cdc_interface;
	usb_desc_set_interface.interface_desc_size = sizeof(cdc_interface);
	usb_desc_set_interface.alt_interface_desc = NULL;
	DEV_IOCTL_1_PARAMS(usb_descriptors_dev,
			USB_DEVICE_DESCRIPTORS_ADD_INTERFACE, &usb_desc_set_interface);

	usb_hw = cfg_hndl->usb_hw;

	set_out_endpoint_callback.callback_dev = adev;
	set_out_endpoint_callback.func = new_data_received;
	set_out_endpoint_callback.endpoint_num = cfg_hndl->endpoint_num;
	DEV_IOCTL_1_PARAMS(usb_hw, IOCTL_USB_DEVICE_SET_OUT_ENDPOINT_CALLBACK,
			&set_out_endpoint_callback);

}


/**
 * usb_virtual_com_class_ioctl()
 *
 * return:
 */
uint8_t usb_virtual_com_class_ioctl(struct dev_desc_t *adev, uint8_t aIoctl_num,
		void * aIoctl_param1, void * aIoctl_param2)
{
	struct usb_virtual_com_class_cfg_t *cfg_hndl;
	struct usb_virtual_com_class_runtime_t *runtime_hndl;

	cfg_hndl = DEV_GET_CONFIG_DATA_POINTER(adev);
	runtime_hndl = DEV_GET_RUNTIME_DATA_POINTER(adev);

	switch(aIoctl_num)
	{


	case IOCTL_DEVICE_START :
		start_virtual_com_class(adev, cfg_hndl, runtime_hndl);
		break;

	default :
		return 1;
	}
	return 0;
}

