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
#define DT_INTERNAL_SRAM_MEMORY_SIZE   0x30000

#define USE_INTERNAL_FLASH
#define DT_INTERNAL_FLASH_NAME          internal_flash
#define DT_INTERNAL_FLASH_BASE_ADDR     0x0
#define DT_INTERNAL_FLASH_MEMORY_SIZE   0x80000

#define CODE_SECTION_LOCATION   internal_flash
#define DATA_SECTION_LOCATION   internal_sram


#include "set_memory_layout.h"

#endif /* */
