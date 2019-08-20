/*
 * file : dt_memory_layout.h
 *
 */

#ifndef dt_memory_layout_h
#define dt_memory_layout_h

#include "project_config.h"

#define USE_INTERNAL_SRAM
#define DT_INTERNAL_SRAM_NAME          internal_sram
#define DT_INTERNAL_SRAM_BASE_ADDR     0xFFFB0000
#if defined(CONFIG_ARBEL_256K_SRAM)
	#define DT_INTERNAL_SRAM_MEMORY_SIZE   0x40000
#else
	#error "Arbel flavor not defined"
#endif

#define USE_EXTERNAL_DDR
#define DT_EXTERNAL_DDR_NAME          external_ddr
#define DT_EXTERNAL_DDR_BASE_ADDR     0x8000
#define DT_EXTERNAL_DDR_MEMORY_SIZE   0x2000000//32MB

#include "set_memory_layout.h"

#endif /* */
