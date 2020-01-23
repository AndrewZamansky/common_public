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

#include "dt_memory_layout.h"

#define RAM_START_ADDR   MEM_BASE_ADDR(internal_sram)

#define  SPIM_CLOCK_ENABLE_REG        *((volatile uint32_t *)0x40000204)
#define  SPIM_DISABLE_SPIM_CACHE_REG  *((volatile uint32_t *)0x40007004)

#define  OMC_BASE_ADDR        0x4000C000
#define  OMC_CTL_REG        *((volatile uint32_t *)OMC_BASE_ADDR)
#define  OMC_ISP_ENABLE    1
#define  OMC_ADDR_REG       *((volatile uint32_t *)(OMC_BASE_ADDR + 0x4))
#define  OMC_CMD_REG       *((volatile uint32_t *)(OMC_BASE_ADDR + 0xC))
#define  OMC_REMAP_CMD    0x2E
#define  OMC_TRG_REG       *((volatile uint32_t *)(OMC_BASE_ADDR + 0x10))
#define  OMC_ISPGO         1

static void remap_vector_table_to_sram()
{
	OMC_CTL_REG = OMC_ISP_ENABLE;
	OMC_ADDR_REG = RAM_START_ADDR;
	OMC_CMD_REG = OMC_REMAP_CMD;
	OMC_TRG_REG |= OMC_ISPGO;
	while (OMC_ISPGO == (OMC_TRG_REG & OMC_ISPGO));
	OMC_CTL_REG &= (~OMC_ISP_ENABLE);
}


uint32_t board_init_before_main_function()
{
    SYS_UnlockReg();
    remap_vector_table_to_sram();
#if !defined(CONFIG_CODE_LOCATION_EXTERNAL_FLASH)
    // enable additional 32k SRAM by disabling SPIM cache
    SPIM_CLOCK_ENABLE_REG |= 0x00004000;
    SPIM_DISABLE_SPIM_CACHE_REG |= 0x00000004;
#endif
	return 0;
}
