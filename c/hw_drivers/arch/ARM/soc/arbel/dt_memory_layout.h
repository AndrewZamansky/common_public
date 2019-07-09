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


#include "set_memory_layout.h"

#endif /* */
