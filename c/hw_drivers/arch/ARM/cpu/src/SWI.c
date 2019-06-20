/* ****************************************
 *****************************************
	@file   SWI.c

	@date   :
	@project  :
	@description :
***************************************** */

/***************   includes    *******************/
#include "project_config.h"
#include "_project.h"
#include "auto_init_api.h"

/***************   defines    *******************/




/***************   typedefs    *******************/



/**********   external variables    **************/

int smihosting_is_active = 0 ;
/***********   loacal variables    **************/


void __attribute__((weak)) OS_SWI_Handler(void)
{
#if (1 == CONFIG_FREE_RTOS)
	EXTERN_C_FUNCTION void vPortSVCHandler( void ) __attribute__ (( naked ));
	#if defined(CONFIG_CORTEX_M3)
		#if defined(CONFIG_GCC)
			__asm volatile
			(
				"	ldr.w r0, =vPortSVCHandler  \n"
				"	ldr.w r14, =0xfffffffd    \n"
				"	bx  r0                   \n "
			);
		#else
			#error "TODO : check if lr needed to be set to 0xfffffffd"
		#endif
	#else
		vPortSVCHandler();
	#endif
#endif
}


/******************** default interrupt handlers ******************/

//EXTERN_C_FUNCTION void
//__attribute__((interrupt("SWI"))) do_app_software_interrupt(uint32_t int_num)
void do_app_software_interrupt(uint32_t int_num)
{
	//based on int_num, you can determine which system call is called
	switch(int_num)
	{
	case 0: /*swi 0 usually is used for OS */
		OS_SWI_Handler();
		break;
	case 1:
		break;
	default:
		break;

    }
}
