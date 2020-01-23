/*
 * file : dt_memory_layout.h
 *
 */

#ifndef dt_memory_layout_h
#define dt_memory_layout_h

#include "project_config.h"

#define USE_INTERNAL_SRAM
#define DT_INTERNAL_SRAM_NAME          internal_sram
#define DT_INTERNAL_SRAM_BASE_ADDR     0x20000000
#if defined(CONFIG_CODE_LOCATION_EXTERNAL_FLASH)
	#define DT_INTERNAL_SRAM_MEMORY_SIZE   0x14000 // 48k+32k
#else
	#define DT_INTERNAL_SRAM_MEMORY_SIZE   0x1C000 // 48k+32k+32k
#endif

#if defined(CONFIG_CODE_LOCATION_EXTERNAL_FLASH)
	#define USE_EXTERNAL_FLASH
	#define DT_EXTERNAL_FLASH_NAME          external_flash
	#define DT_EXTERNAL_FLASH_BASE_ADDR     0x8000000
	#define DT_EXTERNAL_FLASH_MEMORY_SIZE   CONFIG_I96XXX_SPIM_FLASH_SIZE
#endif

#include "set_memory_layout.h"

#endif
