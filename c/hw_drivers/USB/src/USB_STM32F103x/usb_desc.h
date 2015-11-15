/**
  ******************************************************************************
  * @file    usb_desc.h
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    29-June-2012
  * @brief   Descriptor Header for Virtual COM Port Device
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define USB_MAX_DATA_SIZE              64
#define USB_DESC_INT_SIZE               8

#define USB_DESC_SIZ_DEVICE_DESC        18
#define USB_DESC_SIZ_CONFIG_DESC        (67+23+16 )// 23 for storage device +association desc
#define USB_DESC_SIZ_STRING_LANGID      4
#define USB_DESC_SIZ_STRING_VENDOR      38
#define USB_DESC_SIZ_STRING_PRODUCT     52

#define SERIAL_NUMBER_FOR_USB_LEN	8
#define USB_DESC_SIZ_STRING_SERIAL      (12+(2*SERIAL_NUMBER_FOR_USB_LEN)+2)

#define STANDARD_ENDPOINT_DESC_SIZE             0x09

/* Exported functions ------------------------------------------------------- */
extern const uint8_t USB_DESC_DeviceDescriptor[USB_DESC_SIZ_DEVICE_DESC];
extern const uint8_t USB_DESC_ConfigDescriptor[USB_DESC_SIZ_CONFIG_DESC];

extern const uint8_t USB_DESC_StringLangID[USB_DESC_SIZ_STRING_LANGID];
extern const uint8_t USB_DESC_StringVendor[USB_DESC_SIZ_STRING_VENDOR];
extern const uint8_t USB_DESC_StringProduct[USB_DESC_SIZ_STRING_PRODUCT];
extern uint8_t USB_DESC_StringSerial[USB_DESC_SIZ_STRING_SERIAL];
extern const uint8_t USB_DESC_StringStorage[USB_DESC_SIZ_STRING_PRODUCT] ;

#endif /* __USB_DESC_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
