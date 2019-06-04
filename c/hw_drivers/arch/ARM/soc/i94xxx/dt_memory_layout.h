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
#if defined(CONFIG_I941X4_196K_SRAM)
	#define DT_INTERNAL_SRAM_MEMORY_SIZE   0x30000
#elif defined(CONFIG_I941X3_128K_SRAM)
	#define DT_INTERNAL_SRAM_MEMORY_SIZE   0x20000
#elif defined(CONFIG_I941X1_64K_SRAM)
	#define DT_INTERNAL_SRAM_MEMORY_SIZE   0x10000
#elif defined(CONFIG_I941X0_48K_SRAM)
	#define DT_INTERNAL_SRAM_MEMORY_SIZE   0xc000
#else
	#error "I94XXX flavor not defined"
#endif

#define USE_INTERNAL_FLASH
#define DT_INTERNAL_FLASH_NAME          internal_flash
#define DT_INTERNAL_FLASH_BASE_ADDR     0x0
#if defined(CONFIG_I9412X_512K_FLASH)
	#define DT_INTERNAL_FLASH_MEMORY_SIZE   0x80000
#elif defined(CONFIG_I9411X_256K_FLASH)
	#define DT_INTERNAL_FLASH_MEMORY_SIZE   0x40000
#elif defined(CONFIG_I9413X_128K_FLASH)
	#define DT_INTERNAL_FLASH_MEMORY_SIZE   0x20000
#else
	#error "I94XXX flavor not defined"
#endif


#include "set_memory_layout.h"

#endif /* */
