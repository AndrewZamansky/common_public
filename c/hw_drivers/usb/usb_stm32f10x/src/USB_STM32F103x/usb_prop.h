/**
  ******************************************************************************
  * @file    usb_prop.h
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    29-June-2012
  * @brief   All processing related to Virtual COM Port Demo (Endpoint 0)
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
#ifndef __usb_prop_H
#define __usb_prop_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/

#define USB_Request_GetConfiguration          NOP_Process
//#define USB_Request_SetConfiguration          NOP_Process
#define USB_Request_GetInterface              NOP_Process
#define USB_Request_SetInterface              NOP_Process
#define USB_Request_GetStatus                 NOP_Process
//#define USB_Request_ClearFeature              NOP_Process
#define USB_Request_SetEndPointFeature        NOP_Process
#define USB_Request_SetDeviceFeature          NOP_Process
//#define USB_Request_SetDeviceAddress          NOP_Process

#define SEND_ENCAPSULATED_COMMAND   0x00
#define GET_ENCAPSULATED_RESPONSE   0x01
#define SET_COMM_FEATURE            0x02
#define GET_COMM_FEATURE            0x03
#define CLEAR_COMM_FEATURE          0x04
#define SET_LINE_CODING             0x20
#define GET_LINE_CODING             0x21
#define SET_CONTROL_LINE_STATE      0x22
#define SEND_BREAK                  0x23

/* MASS Storage Requests*/
#define GET_MAX_LUN                0xFE
#define MASS_STORAGE_RESET         0xFF
#define LUN_DATA_LENGTH            1

/* Exported functions ------------------------------------------------------- */
void USB_Request_init(void);
void USB_Request_Reset(void);
void USB_Request_SetConfiguration(void);
void USB_Request_SetDeviceAddress (void);
void USB_Request_Status_In (void);
void USB_Request_Status_Out (void);
RESULT USB_Request_Data_Setup(uint8_t);
RESULT USB_Request_NoData_Setup(uint8_t);
RESULT USB_Request_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *USB_Request_GetDeviceDescriptor(uint16_t );
uint8_t *USB_Request_GetConfigDescriptor(uint16_t);
uint8_t *USB_Request_GetStringDescriptor(uint16_t);

uint8_t *Virtual_Com_Port_GetLineCoding(uint16_t Length);
uint8_t *Virtual_Com_Port_SetLineCoding(uint16_t Length);
uint8_t *Get_Max_Lun(uint16_t Length);

#endif /* __usb_prop_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

