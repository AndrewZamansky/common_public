#include "dt_memory_layout.h"

MEMORY {
#ifdef USE_INTERNAL_FLASH
	internal_flash : ORIGIN = MEM_BASE_ADDR(internal_flash), LENGTH = MEM_SIZE(internal_flash)
#endif
#ifdef USE_EXTERNAL_FLASH
	external_flash : ORIGIN = MEM_BASE_ADDR(external_flash), LENGTH = MEM_SIZE(external_flash)
#endif
#ifdef USE_INTERNAL_SRAM
	internal_sram : ORIGIN = MEM_BASE_ADDR(internal_sram), LENGTH = MEM_SIZE(internal_sram)
#endif
#ifdef USE_EXTERNAL_DDR
	ddr : ORIGIN = MEM_BASE_ADDR(external_ddr), LENGTH = MEM_SIZE(external_ddr)
#endif
    /* The following section is for checking illegal sections */
    dummy_area    :   ORIGIN = 0xFFFFFFFFFFFFFFFF,           LENGTH = 0
}
