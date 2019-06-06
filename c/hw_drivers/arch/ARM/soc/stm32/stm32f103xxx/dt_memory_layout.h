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
#if defined(CONFIG_STM32F103xC_64K_SRAM)
	#define DT_INTERNAL_SRAM_MEMORY_SIZE   0x10000
#elif defined(CONFIG_STM32F103xF_96K_SRAM)
	#define DT_INTERNAL_SRAM_MEMORY_SIZE   0x18000
#else
	#error "STM32F103X flavor not defined"
#endif

#define USE_INTERNAL_FLASH
#define DT_INTERNAL_FLASH_NAME          internal_flash
#define DT_INTERNAL_FLASH_BASE_ADDR     0x8000000
#if defined(CONFIG_STM32F103xC_256K_FLASH)
	#define DT_INTERNAL_FLASH_MEMORY_SIZE   0x40000
#elif defined(CONFIG_STM32F103xD_384K_FLASH)
	#define DT_INTERNAL_FLASH_MEMORY_SIZE   0x60000
#elif defined(CONFIG_STM32F103xE_512K_FLASH)
	#define DT_INTERNAL_FLASH_MEMORY_SIZE   0x80000
#elif defined(CONFIG_STM32F103xF_768K_FLASH)
	#define DT_INTERNAL_FLASH_MEMORY_SIZE   0xC0000
#else
	#error "STM32F103X flavor not defined"
#endif

#include "set_memory_layout.h"

#endif /* */
