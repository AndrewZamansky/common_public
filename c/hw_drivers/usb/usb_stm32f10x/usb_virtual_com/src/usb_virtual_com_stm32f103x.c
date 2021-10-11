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

#include "usb_lib.h"
#include "src/USB_STM32F103x/usb_conf.h"  // relative to  _hw_drivers/USB
#include "src/USB_STM32F103x/usb_desc.h"  // relative to  _hw_drivers/USB
#include "uart_api.h"



struct LINE_CODING {
  uint32_t bitrate;
  uint8_t format;
  uint8_t paritytype;
  uint8_t datatype;
};


static struct dev_desc_t *   callback_dev;

static uint32_t sentLen;


static uint8_t USB_Rx_Buffer[USB_MAX_DATA_SIZE];



struct LINE_CODING linecoding =
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
	DEV_CALLBACK(callback_dev , CALLBACK_TX_DONE, (void*)sentLen);
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
	uint32_t USB_Rx_Cnt;
	/* Get the received data buffer and update the counter */
	USB_Rx_Cnt = USB_SIL_Read(EP3_OUT, USB_Rx_Buffer);
	DEV_CALLBACK(
		callback_dev, CALLBACK_DATA_RECEIVED, USB_Rx_Buffer, (void*)USB_Rx_Cnt);

#ifndef STM32F10X_CL
  /* Enable the receive of data on EP3 */
  SetEPRxValid(ENDP3);
#endif /* STM32F10X_CL */
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
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
    return NULL;
  }
  return(uint8_t *)&linecoding;
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
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
    return NULL;
  }
  return(uint8_t *)&linecoding;
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
    if (aLength > USB_MAX_DATA_SIZE)
    {
    	sentLen = USB_MAX_DATA_SIZE;
    }
    else
    {
    	sentLen = aLength;
    }

#ifdef USE_STM3210C_EVAL
    USB_SIL_Write(EP1_IN,apData, lCurrent_pos);
#else
    UserToPMABufferCopy(( uint8_t *)apData, ENDP1_TXADDR, sentLen);
    SetEPTxCount(ENDP1, sentLen);
    SetEPTxValid(ENDP1);
#endif

    return sentLen;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        usb_virtual_com_ioctl                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t usb_virtual_com_ioctl( struct dev_desc_t *adev ,const uint8_t aIoctl_num
		, void * aIoctl_param1 , void * aIoctl_param2)
{
	switch(aIoctl_num)
	{
		case IOCTL_GET_PARAMS_ARRAY_FUNC :
			*(uint8_t*)aIoctl_param2 =  0; //size
			break;

		case IOCTL_DEVICE_START :

			break;
		case IOCTL_UART_DISABLE_TX :
		    SetEPTxStatus(ENDP1, EP_TX_NAK);
			break;
		case IOCTL_SET_ISR_CALLBACK_DEV:
			callback_dev =(struct dev_desc_t *) aIoctl_param1;
			break;
		default :
			return 1;
	}
	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        usb_virtual_com_api_init_dev_descriptor                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t  usb_virtual_com_api_init_dev_descriptor(struct dev_desc_t *aDevDescriptor)
{
	if(NULL == aDevDescriptor) return 1;


	aDevDescriptor->handle = NULL;
	aDevDescriptor->ioctl = usb_virtual_com_ioctl;
	aDevDescriptor->pwrite = usb_virtual_com_pwrite;

	return 0 ;

}

