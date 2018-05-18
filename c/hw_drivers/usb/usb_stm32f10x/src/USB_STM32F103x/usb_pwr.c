/**
  ******************************************************************************
  * @file    usb_pwr.c
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    29-June-2012
  * @brief   Connection/disconnection & power management
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

#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_pwr.h"
#include "irq_api.h"
//#include "timer_api.h"
#include "USB_api.h"
#include "../USB.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t bDeviceState = UNCONNECTED; /* USB device status */
//__IO bool fSuspendEnabled = TRUE;  /* true when suspend is possible */
__IO bool fSuspendEnabled = FALSE;  /* az true when suspend is possible */

struct
{
  __IO RESUME_STATE eState;
  __IO uint8_t bESOFcnt;
}ResumeS;

/* Extern variables ----------------------------------------------------------*/

extern USB_Instance_t USB_InstanceParams;


/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : Power-off system clocks and power while entering suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
  bDeviceState = SUSPENDED;
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else
  {
    bDeviceState = ATTACHED;
  }
}


void My_USB_Resume( );

/*******************************************************************************
* Function Name  : PowerOn
* Description    :
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
RESULT PowerOn(void)
{
#ifndef STM32F10X_CL
  uint16_t wRegVal;

  /*** cable plugged-in ? ***/
 // USB_Cable_Config(ENABLE);

  /*** CNTR_PWDN = 0 ***/
  wRegVal = CNTR_FRES;
  _SetCNTR(wRegVal);

  /*** CNTR_FRES = 0 ***/
  wInterrupt_Mask = 0;
  _SetCNTR(wInterrupt_Mask);
  /*** Clear pending interrupts ***/
  _SetISTR(0);
  /*** Set interrupt mask ***/
  wInterrupt_Mask = IMR_MSK;// az CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM;
  _SetCNTR(wInterrupt_Mask);

  My_USB_Resume(); // az
#endif /* STM32F10X_CL */

  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : PowerOff
* Description    : handles switch-off conditions
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
RESULT PowerOff()
{
#ifndef STM32F10X_CL
  /* disable all interrupts and force USB reset */
  _SetCNTR(CNTR_FRES);
  /* clear interrupt status register */
  _SetISTR(0);
  /* Disable the Pull-Up*/
  // az USB_Cable_Config(DISABLE);
  /* switch-off device */
  _SetCNTR(CNTR_FRES + CNTR_PDWN);
#endif /* STM32F10X_CL */

  /* sw variables reset */
  /* ... */

  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Suspend
* Description    : sets suspend mode operating conditions
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
void Suspend(void)
{
#ifndef STM32F10X_CL
  uint16_t wCNTR;
  /* suspend preparation */
  /* ... */

  /* macrocell enters suspend mode */
  wCNTR = _GetCNTR();
  wCNTR |= CNTR_FSUSP;
  _SetCNTR(wCNTR);
#endif /* STM32F10X_CL */

  /* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
  /* power reduction */
  /* ... on connected devices */

#ifndef STM32F10X_CL
  /* force low-power mode in the macrocell */
  wCNTR = _GetCNTR();
  wCNTR |= CNTR_LPMODE;
  _SetCNTR(wCNTR);
#endif /* STM32F10X_CL */

  /* switch-off the clocks */
  /* ... */
  Enter_LowPowerMode();

}

/*******************************************************************************
* Function Name  : Resume_Init
* Description    : Handles wake-up restoring normal operations
* Input          : None.
* Output         : None.
* Return         : USB_SUCCESS.
*******************************************************************************/
void Resume_Init(void)
{
#ifndef STM32F10X_CL
  uint16_t wCNTR;
#endif /* STM32F10X_CL */ 

  /* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
  /* restart the clocks */
  /* ...  */

#ifndef STM32F10X_CL
  /* CNTR_LPMODE = 0 */
  wCNTR = _GetCNTR();
  wCNTR &= (~CNTR_LPMODE);
  _SetCNTR(wCNTR);
#endif /* STM32F10X_CL */ 

  /* restore full power */
  /* ... on connected devices */
  Leave_LowPowerMode();

#ifndef STM32F10X_CL
  /* reset FSUSP bit */
  _SetCNTR(IMR_MSK);
#endif /* STM32F10X_CL */

  /* reverse suspend preparation */
  /* ... */

}

/*
 *
 */
void enable_suspend_interrupt()
{
	  uint16_t wCNTR;
	  wCNTR = _GetCNTR();
	  wCNTR |= CNTR_SUSPM;
	  _SetCNTR(wCNTR);
}
//extern uint8_t  usbWasRestarted;

void My_USB_Resume( )
{
#ifndef STM32F10X_CL
  uint16_t wCNTR;
#endif /* STM32F10X_CL */
	irq_disable_interrupt(USB_LP_CAN1_RX0_IRQn);

  wCNTR = _GetCNTR();
  wCNTR |= CNTR_RESUME;
  _SetCNTR(wCNTR);

  busy_delay(5);//should be between 5ms and 15ms


  wCNTR = _GetCNTR();
  wCNTR &= (~CNTR_RESUME);
  _SetCNTR(wCNTR);

  _SetISTR((uint16_t)CLR_ALL);

 // SystemTick_API_StartTimeoutTimer(USB_InstanceParams.usb_stabilize_timer);

	irq_enable_interrupt(USB_LP_CAN1_RX0_IRQn);

 // USB_InstanceParams.do_resume = 2;

}

/*******************************************************************************
* Function Name  : Resume
* Description    : This is the state machine handling resume operations and
*                 timing sequence. The control is based on the Resume structure
*                 variables and on the ESOF interrupt calling this subroutine
*                 without changing machine state.
* Input          : a state machine value (RESUME_STATE)
*                  RESUME_ESOF doesn't change ResumeS.eState allowing
*                  decrementing of the ESOF counter in different states.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Resume(RESUME_STATE eResumeSetVal)
{
	 // if(2!=make_resume)
	USB_InstanceParams.do_resume=1;


#if 1 //az
#else
#ifndef STM32F10X_CL
  uint16_t wCNTR;
#endif /* STM32F10X_CL */

  if (eResumeSetVal != RESUME_ESOF)
    ResumeS.eState = eResumeSetVal;

  switch (ResumeS.eState)
  {
    case RESUME_EXTERNAL:
      Resume_Init();
      ResumeS.eState = RESUME_OFF;
      break;
    case RESUME_INTERNAL:
      Resume_Init();
      ResumeS.eState = RESUME_START;
      break;
    case RESUME_LATER:
      ResumeS.bESOFcnt = 2;
      ResumeS.eState = RESUME_WAIT;
      break;
    case RESUME_WAIT:
      ResumeS.bESOFcnt--;
      if (ResumeS.bESOFcnt == 0)
        ResumeS.eState = RESUME_START;
      break;
    case RESUME_START:
     #ifdef STM32F10X_CL
      OTGD_FS_SetRemoteWakeup();
     #else 
      wCNTR = _GetCNTR();
      wCNTR |= CNTR_RESUME;
      _SetCNTR(wCNTR);
     #endif /* STM32F10X_CL */
      ResumeS.eState = RESUME_ON;
      ResumeS.bESOFcnt = 10;
      break;
    case RESUME_ON:
    #ifndef STM32F10X_CL      
      ResumeS.bESOFcnt--;
      if (ResumeS.bESOFcnt == 0)
      {
     #endif /* STM32F10X_CL */    
       #ifdef STM32F10X_CL
        OTGD_FS_ResetRemoteWakeup();
       #else
        wCNTR = _GetCNTR();
        wCNTR &= (~CNTR_RESUME);
        _SetCNTR(wCNTR);
       #endif /* STM32F10X_CL */
        ResumeS.eState = RESUME_OFF;
     #ifndef STM32F10X_CL
      }
     #endif /* STM32F10X_CL */
      break;
    case RESUME_OFF:
    case RESUME_ESOF:
    default:
      ResumeS.eState = RESUME_OFF;
      break;
  }
#endif

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
