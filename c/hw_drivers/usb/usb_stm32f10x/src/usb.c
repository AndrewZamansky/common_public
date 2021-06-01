/*
 *
 * file :   USB.c
 *
 *
 */



#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h"
#include "PRINTF_api.h"

#include "timer_api.h"
#include "USB_api.h"
#include "USB.h"
//#include "USB_Virtual_COM_API.h"


#define USB_CHECK_TASK_PRIORITY				TASK_NORMAL_PRIORITY

/* The check task uses the sprintf function so requires a little more stack. */
#define USB_TASK_STACK_SIZE			DEFINE_STACK_SIZE( 160 )


struct USB_Instance_t USB_InstanceParams = {0} ;

extern void    My_USB_Resume(void);
//int a0=0,a1=0,a2=0,a3=0,a4=0,a5=0,a6=0,a7=0;
//extern 	void USB_Virtual_COM_APP_Init(void);
extern void enable_suspend_interrupt();


static void USB_Task( void *pvParameters )
{

	switch(USB_InstanceParams.chip)
	{
		case USB_API_Chip_STM32F10x:
			USB_STM32F103x_Init();
			break;
		default :
			return ;
	}


	for( ;; )
	{
		/* Perform this check every mainCHECK_DELAY milliseconds. */
		vTaskDelay( 1000 );
		if(USB_InstanceParams.do_resume == 1)
		{
			//PRINTF_DBG(" usb resume \n" );
			My_USB_Resume();
			USB_InstanceParams.do_resume = 2;
			//USB_Virtual_COM_API_Reset();
		}
		else if (USB_InstanceParams.do_resume == 2)
		{
			enable_suspend_interrupt();
			USB_InstanceParams.do_resume=0;
		}

//PRINTF_DBG("-USB task  tick = %d\n",tick++);
//		if (a0 > 100) {a1=0 ; PRINTF_DBG(" sof > 100\n");}
//		if (a1 > 100) {a1=0 ; PRINTF_DBG(" a1 > 100\n");}
//		if (a2 > 100) {a2=0 ; PRINTF_DBG(" a2 > 100\n");}
//		if (a3 > 100) {a3=0 ; PRINTF_DBG(" a3 > 100\n");}
//		if (a4 > 100) {a4=0 ; PRINTF_DBG(" a4 > 100\n");}
//		if (a5 > 100) {a5=0 ; PRINTF_DBG(" a5 > 100\n");}
//		if (a6 > 100) {a6=0 ; PRINTF_DBG(" susp > 100\n");}
//		if (a7 > 100) {a7=0 ; PRINTF_DBG(" a7 > 100\n");}


		os_stack_test();


	}
}



uint8_t  USB_API_Init(USB_API_Init_params_t *apInit_params)
{
	USB_InstanceParams.chip = apInit_params->chip;
	USB_InstanceParams.do_resume = 2 ;
	//SystemTick_API_CreateTimeoutTimer(&USB_InstanceParams.usb_stabilize_timer,500);



    xTaskCreate( USB_Task,
		"usb_task", USB_TASK_STACK_SIZE, NULL, USB_CHECK_TASK_PRIORITY  , NULL );

    return 0;
}
