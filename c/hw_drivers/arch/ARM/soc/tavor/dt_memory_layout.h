/*
 * file : dt_memory_layout.h
 *
 */

#ifndef dt_memory_layout_h
#define dt_memory_layout_h

#include "project_config.h"

#define USE_INTERNAL_SRAM
#define DT_INTERNAL_SRAM_NAME          internal_sram
#if defined(CONFIG_TAVOR_SRAM_BASE_0x0)
	#define DT_INTERNAL_SRAM_BASE_ADDR     0x00000000
#else
	#define DT_INTERNAL_SRAM_BASE_ADDR     0x1000000
#endif
#if defined(CONFIG_TAVOR_64K_SRAM)
	#define DT_INTERNAL_SRAM_MEMORY_SIZE   0x10000
#elif defined(CONFIG_TAVOR_256K_SRAM)
	#define DT_INTERNAL_SRAM_MEMORY_SIZE   0x40000
#else
	#error "Tavor flavor not defined"
#endif


#define USE_EXTERNAL_DDR
#define DT_EXTERNAL_DDR_NAME          external_ddr
#define DT_EXTERNAL_DDR_BASE_ADDR     0x80000000
#if defined(CONFIG_TAVOR_2MB_DDR)
	#define DT_EXTERNAL_DDR_MEMORY_SIZE   0x200000//2MB
#elif defined(CONFIG_TAVOR_32MB_DDR)
	#define DT_EXTERNAL_DDR_MEMORY_SIZE   0x2000000//32MB
#else
	#error "Tavor flavor not defined"
#endif

#include "set_memory_layout.h"

#endif
