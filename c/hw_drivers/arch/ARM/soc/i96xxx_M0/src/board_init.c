/* ****************************************
 *****************************************
	@file   board_init.c

	@date   :
	@project  :
	@description :
***************************************** */

#include "_project.h"
#include "cpu_config.h"
#include "stdint.h"

#define  SPIM_CLOCK_ENABLE_REG        *((volatile uint32_t *)0x40000204)
#define  SPIM_DISABLE_SPIM_CACHE_REG  *((volatile uint32_t *)0x40007004)

uint32_t board_init_before_main_function()
{
    SYS_UnlockReg();
#if !defined(CONFIG_I96XXX_USE_SPIM_FLASH)
    // enable additional 32k SRAM by disabling SPIM cache
    SPIM_CLOCK_ENABLE_REG |= 0x00004000;
    SPIM_DISABLE_SPIM_CACHE_REG |= 0x00000004;
#endif
	return 0;
}
