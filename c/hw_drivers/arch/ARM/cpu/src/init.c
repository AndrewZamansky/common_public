/* ****************************************
 *****************************************
	@file   low_level_calls.c

	@date   :
	@project  :
	@description :
***************************************** */

#include "_project.h"
#include "auto_init_api.h"

extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __tables_relocation_section_start_on_RAM__;
extern uint32_t __tables_relocation_section_end_on_RAM__;
extern uint32_t __tables_relocation_section_start_on_ROM__;
extern uint32_t __relocation_section_start_on_RAM__;
extern uint32_t __relocation_section_end_on_RAM__;
extern uint32_t __relocation_section_start_on_ROM__;

uint32_t board_init_before_main_function(void);
void core_init(uint32_t curr_stack);



/*
 *  function : init_after_startup()
 */
EXTERN_C_FUNCTION void init_after_startup(uint32_t curr_stack)
{
	uint32_t *src ;
	uint32_t *dst ;


	core_init(curr_stack);
	/* ROM has tables that needs to be copied to RAM. */
	src = &__tables_relocation_section_start_on_ROM__;
	dst = &__tables_relocation_section_start_on_RAM__;
	if (src != dst)
	{
		while (dst < &__tables_relocation_section_end_on_RAM__)
		{
			*dst++ = *src++; /* 4 bytes copy*/
		}
	}

	/* ROM has code and data that needs to be copied to RAM. */
	src = &__relocation_section_start_on_ROM__;
	dst = &__relocation_section_start_on_RAM__;
	if (src != dst)
	{
		while (dst < &__relocation_section_end_on_RAM__)
		{
			*dst++ = *src++; /* 4 bytes copy*/
		}
	}

	/* Zero bss */
	for (dst = &__bss_start__; dst < &__bss_end__; dst++ /* 4 bytes copy*/)
	{
	  *dst = 0;
	}

	board_init_before_main_function();
	auto_init_api();
//	DEV_API_auto_start_devices();
}
