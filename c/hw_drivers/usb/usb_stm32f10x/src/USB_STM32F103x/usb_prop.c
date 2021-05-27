/**
  ******************************************************************************
  * @file    usb_prop.c
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    29-June-2012
  * @brief   All processing related to Virtual Com Port Demo
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"

#include "USB_Mass_Storage/src/usb_bot.h"
//#include "memory.h"
//#include "mass_mal.h"

uint8_t Request = 0;

extern uint8_t *Get_Max_Lun(uint16_t Length);

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
  {
    EP_NUM,
    1
  };

DEVICE_PROP Device_Property =
  {
    USB_Request_init,
    USB_Request_Reset,
    USB_Request_Status_In,
    USB_Request_Status_Out,
    USB_Request_Data_Setup,
    USB_Request_NoData_Setup,
    USB_Request_Get_Interface_Setting,
    USB_Request_GetDeviceDescriptor,
    USB_Request_GetConfigDescriptor,
    USB_Request_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };

void USB_Request_ClearFeature(void);

USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    USB_Request_GetConfiguration,
    USB_Request_SetConfiguration,
    USB_Request_GetInterface,
    USB_Request_SetInterface,
    USB_Request_GetStatus,
   // Mass_Storage_ClearFeature,
    USB_Request_ClearFeature,
    USB_Request_SetEndPointFeature,
    USB_Request_SetDeviceFeature,
    USB_Request_SetDeviceAddress
  };

ONE_DESCRIPTOR Device_Descriptor =
  {
    (uint8_t*)USB_DESC_DeviceDescriptor,
    USB_DESC_SIZ_DEVICE_DESC
  };

ONE_DESCRIPTOR Config_Descriptor =
  {
    (uint8_t*)USB_DESC_ConfigDescriptor,
    USB_DESC_SIZ_CONFIG_DESC
  };

ONE_DESCRIPTOR String_Descriptor[5] =
  {
    {(uint8_t*)USB_DESC_StringLangID, USB_DESC_SIZ_STRING_LANGID},
    {(uint8_t*)USB_DESC_StringVendor, USB_DESC_SIZ_STRING_VENDOR},
    {(uint8_t*)USB_DESC_StringProduct, USB_DESC_SIZ_STRING_PRODUCT},
    {(uint8_t*)USB_DESC_StringSerial, USB_DESC_SIZ_STRING_SERIAL},
    {(uint8_t*)USB_DESC_StringStorage, USB_DESC_SIZ_STRING_PRODUCT}
  };

extern unsigned char Bot_State;
extern Bulk_Only_CBW CBW;


/*******************************************************************************
* Function Name  : USB_Request_init.
* Description    : Virtual COM Port Mouse init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Request_init(void)
{


  pInformation->Current_Configuration = 0;

  /* Connect the device */
  PowerOn();

  /* Perform basic device initialization operations */
  USB_SIL_Init();
#if 0
  /* configure the USART to the default settings */
  USART_Config_Default();
#endif

  bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : USB_Request_Reset
* Description    : USB_Request Mouse reset routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Request_Reset(void)
{
  /* Set USB_Request DEVICE as not configured */
  pInformation->Current_Configuration = 0;

  /* Current Feature initialization */
  pInformation->Current_Feature = USB_DESC_ConfigDescriptor[7];

  /* Set USB_Request DEVICE with the default Interface*/
  pInformation->Current_Interface = 0;

#ifdef STM32F10X_CL     
  /* EP0 is already configured by USB_SIL_Init() function */
  
  /* Init EP1 IN as Bulk endpoint */
  OTG_DEV_EP_Init(EP1_IN, OTG_DEV_EP_TYPE_BULK, USB_MAX_DATA_SIZE);
  
  /* Init EP2 IN as Interrupt endpoint */
  OTG_DEV_EP_Init(EP2_IN, OTG_DEV_EP_TYPE_INT, USB_DESC_INT_SIZE);

  /* Init EP3 OUT as Bulk endpoint */
  OTG_DEV_EP_Init(EP3_OUT, OTG_DEV_EP_TYPE_BULK, USB_MAX_DATA_SIZE);
#else 

  SetBTABLE(BTABLE_ADDRESS);

  /* Initialize Endpoint 0 */
  SetEPType(ENDP0, EP_CONTROL);
  SetEPTxStatus(ENDP0, EP_TX_STALL);
  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  Clear_Status_Out(ENDP0);
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPRxValid(ENDP0);

  /* Initialize Endpoint 1 */
  SetEPType(ENDP1, EP_BULK);
  SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  SetEPTxStatus(ENDP1, EP_TX_NAK);
  SetEPRxStatus(ENDP1, EP_RX_DIS);

  /* Initialize Endpoint 2 */
  SetEPType(ENDP2, EP_INTERRUPT);
  SetEPTxAddr(ENDP2, ENDP2_TXADDR);
  SetEPRxStatus(ENDP2, EP_RX_DIS);
  SetEPTxStatus(ENDP2, EP_TX_NAK);

  /* Initialize Endpoint 3 */
  SetEPType(ENDP3, EP_BULK);
  SetEPRxAddr(ENDP3, ENDP3_RXADDR);
  SetEPRxCount(ENDP3, USB_MAX_DATA_SIZE);
  SetEPRxStatus(ENDP3, EP_RX_VALID);
  SetEPTxStatus(ENDP3, EP_TX_DIS);

  /***************** initialize mass storage endpoints **************/
  /* Initialize Endpoint 4 */
  SetEPType(ENDP4, EP_BULK);
  SetEPTxAddr(ENDP4, ENDP4_TXADDR);
  SetEPTxStatus(ENDP4, EP_TX_NAK);
  SetEPRxStatus(ENDP4, EP_RX_DIS);

  /* Initialize Endpoint 5 */
  SetEPType(ENDP5, EP_BULK);
  SetEPRxAddr(ENDP5, ENDP5_RXADDR);
  SetEPRxCount(ENDP5, Device_Property.MaxPacketSize);
  SetEPRxStatus(ENDP5, EP_RX_VALID);
  SetEPTxStatus(ENDP5, EP_TX_DIS);


  /* Set this device to response on default address */
  SetDeviceAddress(0);
#endif /* STM32F10X_CL */

  CBW.dSignature = BOT_CBW_SIGNATURE;
  Bot_State = BOT_IDLE;

  bDeviceState = ATTACHED;
}

/*******************************************************************************
* Function Name  : USB_Request_SetConfiguration.
* Description    : Update the device state to configured.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Request_SetConfiguration(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;


#ifdef STM32F10X_CL
    /* Init EP1 IN as Bulk endpoint */
    OTG_DEV_EP_Init(EP1_IN, OTG_DEV_EP_TYPE_BULK, BULK_MAX_PACKET_SIZE);

    /* Init EP2 OUT as Bulk endpoint */
    OTG_DEV_EP_Init(EP2_OUT, OTG_DEV_EP_TYPE_BULK, BULK_MAX_PACKET_SIZE);
#else
    ClearDTOG_TX(ENDP4);
    ClearDTOG_RX(ENDP5);
#endif /* STM32F10X_CL */

    Bot_State = BOT_IDLE; /* set the Bot state machine to the IDLE state */
  }
}

/*******************************************************************************
* Function Name  : Mass_Storage_ClearFeature
* Description    : Handle the ClearFeature request.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Request_ClearFeature(void)
//void Mass_Storage_ClearFeature(void)
{
  /* when the host send a CBW with invalid signature or invalid length the two
     Endpoints (IN & OUT) shall stall until receiving a Mass Storage Reset     */
  if (CBW.dSignature != BOT_CBW_SIGNATURE)
    Bot_Abort(BOTH_DIR);
}

/*******************************************************************************
* Function Name  : USB_Request_SetConfiguration.
* Description    : Update the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Request_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}

/*******************************************************************************
* Function Name  : USB_Request_Status_In.
* Description    : Virtual COM Port Status In Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Request_Status_In(void)
{
  if (Request == SET_LINE_CODING)
  {
#if 0 //az
    USART_Config();
#endif
    Request = 0;
  }
}

/*******************************************************************************
* Function Name  : USB_Request_Status_Out
* Description    : Virtual COM Port Status OUT Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Request_Status_Out(void)
{}

/*******************************************************************************
* Function Name  : USB_Request_Data_Setup
* Description    : handle the data class specific requests
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT USB_Request_Data_Setup(uint8_t RequestNo)
{
  uint8_t    *(*CopyRoutine)(uint16_t);

  CopyRoutine = NULL;
  if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
  {
	  if((0 == pInformation->USBwIndex) || (1==pInformation->USBwIndex)) // interface 0 and 1 for virtual com
	  {
		  if (RequestNo == GET_LINE_CODING)
		  {
			  CopyRoutine = Virtual_Com_Port_GetLineCoding;
		  }
		  else if (RequestNo == SET_LINE_CODING)
		  {
			  CopyRoutine = Virtual_Com_Port_SetLineCoding;
			  Request = SET_LINE_CODING;
		  }
	  }
	  else if (2 == pInformation->USBwIndex)  // interface 2 for mass storage
	  {
		  if (RequestNo == GET_MAX_LUN)
		  {
			  if ( (pInformation->USBwValue == 0) && (pInformation->USBwLength == 0x01))
			  {
				CopyRoutine = Get_Max_Lun;
			  }
		  }
	  }
  }
  if (CopyRoutine == NULL)
  {
    return USB_UNSUPPORT;
  }

  pInformation->Ctrl_Info.CopyData = CopyRoutine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*CopyRoutine)(0);
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : USB_Request_NoData_Setup.
* Description    : handle the no data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT USB_Request_NoData_Setup(uint8_t RequestNo)
{

  if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
  {
	  if((0 == pInformation->USBwIndex) || (1==pInformation->USBwIndex)) // interface 0 and 1 for virtual com
	  {
		if (RequestNo == SET_COMM_FEATURE)
		{
		  return USB_SUCCESS;
		}
		else if (RequestNo == SET_CONTROL_LINE_STATE)
		{
		  return USB_SUCCESS;
		}
	  }
	  else if (2 == pInformation->USBwIndex)  // interface 2 for mass storage
	  {
		   if ((RequestNo == MASS_STORAGE_RESET) && (pInformation->USBwValue == 0)
				/*& (pInformation->USBwIndex == 0)*/ && (pInformation->USBwLength == 0x00))
			{
			 #ifdef STM32F10X_CL
			  /* Init EP1 IN as Bulk endpoint */
			  OTG_DEV_EP_Init(EP1_IN, OTG_DEV_EP_TYPE_BULK, BULK_MAX_PACKET_SIZE);

			  /* Init EP2 OUT as Bulk endpoint */
			  OTG_DEV_EP_Init(EP2_OUT, OTG_DEV_EP_TYPE_BULK, BULK_MAX_PACKET_SIZE);
			 #else
			  /* Initialize Endpoint 1 */
			  ClearDTOG_TX(ENDP4);

			  /* Initialize Endpoint 2 */
			  ClearDTOG_RX(ENDP5);
			 #endif /* STM32F10X_CL */

			  /*initialize the CBW signature to enable the clear feature*/
			  CBW.dSignature = BOT_CBW_SIGNATURE;
			  Bot_State = BOT_IDLE;

			  return USB_SUCCESS;
			}
	  }
  }

  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : USB_Request_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
uint8_t *USB_Request_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : USB_Request_GetConfigDescriptor.
* Description    : get the configuration descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *USB_Request_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : USB_Request_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *USB_Request_GetStringDescriptor(uint16_t Length)
{
  uint8_t wValue0 = pInformation->USBwValue0;
  if (wValue0 > 4)
  {
    return NULL;
  }
  else
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
}

/*******************************************************************************
* Function Name  : USB_Request_Get_Interface_Setting.
* Description    : test the interface and the alternate setting according to the
*                  supported one.
* Input1         : uint8_t: Interface : interface number.
* Input2         : uint8_t: AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
RESULT USB_Request_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 1)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

