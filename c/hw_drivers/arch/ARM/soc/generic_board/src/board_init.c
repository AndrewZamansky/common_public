/* ****************************************
 *****************************************
	@file   board_init.c

	@date   :
	@project  :
	@description :
***************************************** */

#include "_project.h"
#include "cpu_config.h"
#ifdef CONFIG_CORTEX_M4
	#include "core_cm4.h"  /* Cortex-M4 processor and core peripherals   */
#endif

int running_on_palladium = 0 ;

uint32_t board_init_before_main_function()
{
#ifdef CONFIG_CORTEX_M4
	/* FPU settings -------------------*/
	#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
		SCB->CPACR |= ((3UL << 10*2) |            /* set CP10 Full Access */
					(3UL << 11*2)  );             /* set CP11 Full Access */

	#endif
#endif

	return 0;
}
