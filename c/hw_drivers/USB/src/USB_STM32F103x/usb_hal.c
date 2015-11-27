/*
 *
 * USB_hal.c
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

#include "dev_managment_api.h" // for device manager defines and typedefs


#include "USB_api.h"
#include "serial_number_stm32f10x_api.h"

#include "stm32f10x_rcc.h"
#include "misc.h"
#include "NVIC_api.h"
#include "usb_lib.h"
#include "usb_init.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_crc_stm32f10x_api.h"

#define usb_hush32_function  hw_crc_stm32f10x_api


/********  defines *********************/


/********  types  *********************/


/********  externals *********************/

extern void USB_Istr(void);

/********  local defs *********************/
/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             5

///*******************************************************************************
//* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
//* Description    :
//* Input          : None.
//* Output         : None.
//* Return         : None.
//*******************************************************************************/
//#ifdef STM32F10X_CL
//void INTR_SOFINTR_Callback(void)
//#else
//void SOF_Callback(void)
//#endif /* STM32F10X_CL */
//{
//  static uint32_t FrameCount = 0;
//
//  if(bDeviceState == CONFIGURED)
//  {
//    if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
//    {
//      /* Reset the frame counter */
//      FrameCount = 0;
//
//      /* Check the data to be sent through IN pipe */
//      //   Handle_USBAsynchXfer();
//    }
//  }
//}


/*******************************************************************************
* Function Name  : HexToChar.
* Description    : Convert Hex 32Bits value into char.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;

  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10;
    }

    value = value << 4;

    pbuf[ 2* idx + 1] = 0;
  }
}

//
///*******************************************************************************
//* Function Name  : Get_SerialNum.
//* Description    : Create the serial number string descriptor.
//* Input          : None.
//* Output         : None.
//* Return         : None.
//*******************************************************************************/
//void Update_SerialNum(void)
//{
//  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
//
//#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD) || defined(STM32L1XX_MD_PLUS)
//  Device_Serial0 = *(uint32_t*)(0x1FF80050);
//  Device_Serial1 = *(uint32_t*)(0x1FF80054);
//  Device_Serial2 = *(uint32_t*)(0x1FF80064);
//#else
//  Device_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
//  Device_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
//  Device_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);
//#endif /* STM32L1XX_XD */
//
//  Device_Serial0 += Device_Serial2;
//
//  if (Device_Serial0 != 0)
//  {
//    IntToUnicode (Device_Serial0, &USB_DESC_StringSerial[2] , 8);
//    IntToUnicode (Device_Serial1, &USB_DESC_StringSerial[18], 4);
//  }
//}
//

extern uint8_t *pSerNum;

///*******************************************************************************
//* Function Name  : IntNumToCharNum.
//* Description    :
//* Input          : None.
//* Output         : None.
//* Return         : None.
//*******************************************************************************/
//static uint8_t IntNumToCharNum (uint32_t value , uint8_t *pbuf )
//{
//  uint8_t idx = 0;
//  uint8_t currDigit;
//
//  for( idx = 0 ; idx < 8 ; idx ++)
//  {
//	currDigit= value & 0xf;
//    if( currDigit < 0xA )
//    {
//      pbuf[ idx] = currDigit + '0';
//    }
//    else
//    {
//      pbuf[idx] = currDigit + 'A' - 10;
//    }
//
//    value = value >> 4;
//  }
//}


void    USB_STM32F103x_Init(void)
{
	uint32_t hashedSerialNum;


	//Update_SerialNum();
	//uint8_t idx ;

	hashedSerialNum = usb_hush32_function(pSerNum,SERIAL_NUMBER_LEN);

	IntToUnicode(hashedSerialNum,&USB_DESC_StringSerial[12],SERIAL_NUMBER_FOR_USB_LEN);
	// update serial number string


	/* Select USBCLK source */
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

	/* Enable the USB clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);



	NVIC_API_RegisterInt(USB_LP_CAN1_RX0_IRQn , USB_Istr);
	NVIC_API_SetPriority(USB_LP_CAN1_RX0_IRQn , INTERRUPT_LOWEST_PRIORITY - 1 );
	NVIC_API_EnableInt(USB_LP_CAN1_RX0_IRQn);

	USB_Init();


}



