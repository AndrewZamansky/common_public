/*
 * file : set_memory_layout.h
 *
 */

#ifndef set_memory_layout_h
#define set_memory_layout_h

#define  internal_sram   1
#define  internal_flash  2
#define  external_ddr    3


#ifdef USE_INTERNAL_SRAM
	#ifndef DT_INTERNAL_SRAM_NAME
		#error "DT_INTERNAL_SRAM_NAME should be defined as :internal_sram"
	#endif
	#if ( internal_sram != DT_INTERNAL_SRAM_NAME)
		#error "DT_INTERNAL_SRAM_NAME should be defined as :internal_sram"
	#endif
	#ifndef DT_INTERNAL_SRAM_BASE_ADDR
		#error "DT_INTERNAL_SRAM_BASE_ADDR should be defined"
	#endif
	#ifndef DT_INTERNAL_SRAM_MEMORY_SIZE
		#error "DT_INTERNAL_SRAM_MEMORY_SIZE should be defined"
	#endif
	#define  internal_sram_BASE_ADDR  DT_INTERNAL_SRAM_BASE_ADDR
	#define  internal_sram_SIZE       DT_INTERNAL_SRAM_MEMORY_SIZE
	#if (internal_sram == CODE_SECTION_LOCATION)
		#define   CODE_SECTION_LOCATION_INTERNAL_SRAM
	#endif
	#if (internal_sram == DATA_SECTION_LOCATION)
		#define   DATA_SECTION_LOCATION_INTERNAL_SRAM
	#endif
#endif


#ifdef USE_INTERNAL_FLASH
	#ifndef DT_INTERNAL_FLASH_NAME
		#error "DT_INTERNAL_FLASH_NAME should be defined as :internal_flash"
	#endif
	#if ( internal_flash != DT_INTERNAL_FLASH_NAME)
		#error "DT_INTERNAL_FLASH_NAME should be defined as :internal_flash"
	#endif
	#ifndef DT_INTERNAL_FLASH_BASE_ADDR
		#error "DT_INTERNAL_FLASH_BASE_ADDR should be defined"
	#endif
	#ifndef DT_INTERNAL_FLASH_MEMORY_SIZE
		#error "DT_INTERNAL_FLASH_MEMORY_SIZE should be defined"
	#endif
	#define  internal_flash_BASE_ADDR  DT_INTERNAL_FLASH_BASE_ADDR
	#define  internal_flash_SIZE       DT_INTERNAL_FLASH_MEMORY_SIZE
	#if (internal_flash == CODE_SECTION_LOCATION)
		#define   CODE_SECTION_LOCATION_INTERNAL_FLASH
	#endif
#endif


#ifdef USE_EXTERNAL_DDR
	#ifndef DT_EXTERNAL_DDR_NAME
		#error "DT_EXTERNAL_DDR_NAME should be defined as :external_ddr"
	#endif
	#if ( external_ddr != DT_EXTERNAL_DDR_NAME)
		#error "DT_EXTERNAL_DDR_NAME should be defined as :external_ddr"
	#endif
	#ifndef DT_EXTERNAL_DDR_BASE_ADDR
		#error "DT_EXTERNAL_DDR_BASE_ADDR should be defined"
	#endif
	#ifndef DT_EXTERNAL_DDR_MEMORY_SIZE
		#error "DT_EXTERNAL_DDR_MEMORY_SIZE should be defined"
	#endif
	#define  external_ddr_BASE_ADDR  DT_EXTERNAL_DDR_BASE_ADDR
	#define  external_ddr_SIZE       DT_EXTERNAL_DDR_MEMORY_SIZE
	#if (external_ddr == CODE_SECTION_LOCATION)
		#define   CODE_SECTION_LOCATION_EXTERNAL_DDR
	#endif
	#if (external_ddr == DATA_SECTION_LOCATION)
		#define   DATA_SECTION_LOCATION_EXTERNAL_DDR
	#endif
#endif



#define MEM_BASE_ADDR(mem)  mem ## _BASE_ADDR
#define MEM_SIZE(mem)       mem ## _SIZE

#undef  internal_sram
#undef  internal_flash
#undef  external_ddr

#endif /* */
