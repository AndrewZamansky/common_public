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


/******************** default interrupt handlers ******************/

EXTERN_C_FUNCTION void
__attribute__((interrupt("SWI"))) do_app_software_interrupt(uint32_t int_num)
{
    //based on int_num, you can determine which system call is called
    switch(int_num)
    {
    	case 1:
    		break;
    	default:
    		break;

    }
}


void __attribute__((weak)) OS_SWI_Handler(void)
{
#if (1 == CONFIG_FREE_RTOS)
	EXTERN_C_FUNCTION void vPortSVCHandler( void ) __attribute__ (( naked ));
	vPortSVCHandler();
#endif
}

