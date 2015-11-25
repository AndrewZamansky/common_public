/**
  ******************************************************************************
  * @file    memory.c
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    29-June-2012
  * @brief   Memory management layer
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


/* Includes ------------------------------------------------------------------*/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_managment_api.h" // for device manager defines and typedefs

#include "memory.h"
#include "usb_scsi.h"
#include "usb_bot.h"
#include "usb_regs.h"
#include "usb_mem.h"
//#include "mass_mal.h"
#include "usb_lib.h"
//#include "Storage_API.h"
#include "USB_Mass_Storage_api.h"
#include "USB_Mass_Storage.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t Block_Read_count = 0;
__IO uint32_t Block_offset;
__IO uint32_t Counter = 0;
uint32_t  Idx;
uint8_t Data_Buffer[512];  //[BULK_MAX_PACKET_SIZE *2]; /* 512 bytes temporary*/
//uint32_t Data_Buffer[BULK_MAX_PACKET_SIZE *2]; /* 512 bytes temporary*/
uint8_t TransferState = TXFR_IDLE;
/* Extern variables ----------------------------------------------------------*/
extern uint8_t Bulk_Data_Buff[BULK_MAX_PACKET_SIZE];  /* data buffer*/
extern uint16_t Data_Len;
extern uint8_t Bot_State;
extern Bulk_Only_CBW CBW;
extern Bulk_Only_CSW CSW;
extern USB_Mass_Storage_Instance_t USB_Mass_Storage_InstanceParams ;

//extern uint32_t Mass_Block_Size[2];

/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : Read_Memory
* Description    : Handle the Read operation from the microSD card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Read_Memory(uint8_t lun, uint32_t Memory_Offset, uint32_t Transfer_Length)
{
  static uint32_t Offset, Length;

  if (TransferState == TXFR_IDLE )
  {
//	    Offset = Memory_Offset * Mass_Block_Size[lun];
//	    Length = Transfer_Length * Mass_Block_Size[lun];
	    Offset = Memory_Offset * USB_Mass_Storage_InstanceParams.BlockSize;
	    Length = Transfer_Length * USB_Mass_Storage_InstanceParams.BlockSize;
    TransferState = TXFR_ONGOING;
  }

  if (TransferState == TXFR_ONGOING )
  {
    if (!Block_Read_count)
    {
    	DEV_PREAD(USB_Mass_Storage_InstanceParams.storage_dev,
    			 Data_Buffer , USB_Mass_Storage_InstanceParams.BlockSize,Offset);
//      MAL_Read(lun ,
//               Offset ,
//               Data_Buffer,
//               Mass_Block_Size[lun]);

      USB_SIL_Write(EP4_IN, (uint8_t *)Data_Buffer, BULK_MAX_PACKET_SIZE);

      Block_Read_count = USB_Mass_Storage_InstanceParams.BlockSize - BULK_MAX_PACKET_SIZE;
      Block_offset = BULK_MAX_PACKET_SIZE;
    }
    else
    {
      USB_SIL_Write(EP4_IN, (uint8_t *)Data_Buffer + Block_offset, BULK_MAX_PACKET_SIZE);

      Block_Read_count -= BULK_MAX_PACKET_SIZE;
      Block_offset += BULK_MAX_PACKET_SIZE;
    }

    SetEPTxCount(ENDP4, BULK_MAX_PACKET_SIZE);
#ifndef USE_STM3210C_EVAL
    SetEPTxStatus(ENDP4, EP_TX_VALID);
#endif    
    Offset += BULK_MAX_PACKET_SIZE;
    Length -= BULK_MAX_PACKET_SIZE;

    CSW.dDataResidue -= BULK_MAX_PACKET_SIZE;
    //Led_RW_ON();
  }
  if (Length == 0)
  {
    Block_Read_count = 0;
    Block_offset = 0;
    Offset = 0;
    Bot_State = BOT_DATA_IN_LAST;
    TransferState = TXFR_IDLE;
   // Led_RW_OFF();
  }
}

/*******************************************************************************
* Function Name  : Write_Memory
* Description    : Handle the Write operation to the microSD card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Write_Memory (uint8_t lun, uint32_t Memory_Offset, uint32_t Transfer_Length)
{

  static uint32_t W_Offset, W_Length;

  uint32_t temp =  Counter + 64;

  if (TransferState == TXFR_IDLE )
  {
//    W_Offset = Memory_Offset * Mass_Block_Size[lun];
//    W_Length = Transfer_Length * Mass_Block_Size[lun];
	  W_Offset = Memory_Offset * USB_Mass_Storage_InstanceParams.BlockSize;
	  W_Length = Transfer_Length * USB_Mass_Storage_InstanceParams.BlockSize;
    TransferState = TXFR_ONGOING;
  }

  if (TransferState == TXFR_ONGOING )
  {

    for (Idx = 0 ; Counter < temp; Counter++)
    {
      *((uint8_t *)Data_Buffer + Counter) = Bulk_Data_Buff[Idx++];
    }

    W_Offset += Data_Len;
    W_Length -= Data_Len;

//    if (!(W_Length % Mass_Block_Size[lun]))
    if (!(W_Length % USB_Mass_Storage_InstanceParams.BlockSize))
    {
      Counter = 0;
      DEV_PWRITE(USB_Mass_Storage_InstanceParams.storage_dev ,
  			 Data_Buffer , USB_Mass_Storage_InstanceParams.BlockSize,
  			W_Offset -USB_Mass_Storage_InstanceParams.BlockSize);
//      MAL_Write(lun ,
//                W_Offset - Mass_Block_Size[lun],
//                Data_Buffer,
//                Mass_Block_Size[lun]);
    }

    CSW.dDataResidue -= Data_Len;
  #ifndef STM32F10X_CL
    SetEPRxStatus(ENDP5, EP_RX_VALID); /* enable the next transaction*/
  #endif /* STM32F10X_CL */

 //   Led_RW_ON();
  }

  if ((W_Length == 0) || (Bot_State == BOT_CSW_Send))
  {
    Counter = 0;
    Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
    TransferState = TXFR_IDLE;
 //   Led_RW_OFF();
  }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

