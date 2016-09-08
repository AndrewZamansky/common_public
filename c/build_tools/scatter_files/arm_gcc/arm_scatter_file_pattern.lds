#include "project_config.h"

ENTRY(_secondary_rom_vector_table_startup_entry)

MEMORY {
#ifdef CONFIG_INTERNAL_FLASH
	internal_flash	:   ORIGIN = CONFIG_DT_SOC_FLASH_START_ADDR, 	LENGTH = CONFIG_DT_SOC_FLASH_SIZE
#endif
#ifdef CONFIG_INTERNAL_SRAM
	internal_sram  	:   ORIGIN = CONFIG_DT_SOC_RAM_START_ADDR ,   	LENGTH =  CONFIG_DT_SOC_RAM_SIZE
#endif    
#ifdef CONFIG_DDR
	ddr  	:   ORIGIN = CONFIG_DDR_START_ADDR ,   	LENGTH =  CONFIG_DDR_SIZE
#endif    
    /* The following section is for checking illegal sections */
    dummy_area    :   ORIGIN = 0xFFFFFFFFFFFFFFFF,           LENGTH = 0
}

#ifdef CORTEX_M
	STACK_SIZE = 1024;
#else /*cortex-a*/
	SYS_STACK_SIZE = 128;
	IRQ_STACK_SIZE = 1024;
	FIQ_STACK_SIZE = 128;
	SVC_STACK_SIZE = 1024;
	ABT_STACK_SIZE = 128;
	UND_STACK_SIZE = 128;
#endif
	EXCEPTION_VECTOR_AREA_SIZE  	= 0x100;
	SECONDARY_RAM_VECTOR_AREA_SIZE  = 0x40;//with some extra for future use

SECTIONS
{
#ifdef CONFIG_DATA_LOCATION_INTERNAL_SRAM
	#define DATA_LOCATION internal_sram
	__RAM_START__ = ORIGIN(internal_sram);
#endif
#ifdef CONFIG_DATA_LOCATION_DDR
	#define DATA_LOCATION ddr
	__RAM_START__ = ORIGIN(ddr);
#endif

#ifdef CONFIG_CODE_LOCATION_INTERNAL_FLASH
	#define CODE_LOCATION internal_flash
	__ROM_START__ = ORIGIN(internal_flash);
	__tables_relocation_section_start_on_ROM__ =  __ROM_START__ ;
#endif
	
#ifdef CONFIG_CODE_LOCATION_INTERNAL_SRAM
	#define CODE_LOCATION internal_sram
	__tables_relocation_section_start_on_ROM__ = __RAM_START__ ;
#endif

#ifdef CONFIG_CODE_LOCATION_DDR
	#define CODE_LOCATION ddr
	__tables_relocation_section_start_on_ROM__ = __RAM_START__ ;
#endif

	__tables_relocation_section_start_on_RAM__ = __RAM_START__;

	/* the AT process of first table is not used because it cause problem on nuc-link debugger to rom it : */
	/*.vector_table __tables_relocation_section_start_on_RAM__ : AT ( __tables_relocation_section_start_on_ROM__ ) */
	.vector_table __tables_relocation_section_start_on_ROM__ :
	{ 
		KEEP(*(._arm_vector_table))
	} > CODE_LOCATION

	__secondary_RAM_vector_table_start_on_ROM__ = __tables_relocation_section_start_on_ROM__ + EXCEPTION_VECTOR_AREA_SIZE ;
	__secondary_RAM_vector_table_start_on_RAM__ = __tables_relocation_section_start_on_RAM__ + EXCEPTION_VECTOR_AREA_SIZE ;
	.secondary_ram_vector_table __secondary_RAM_vector_table_start_on_RAM__ : AT ( __secondary_RAM_vector_table_start_on_ROM__ ) 
	{ 
	    KEEP(*(._secondary_ram_vector_table))
	} > DATA_LOCATION

	/********************************/
	/*******  device tree **********/
	__start_of_device_tree_on_ROM__ = __secondary_RAM_vector_table_start_on_ROM__  + SECONDARY_RAM_VECTOR_AREA_SIZE ;
	__start_of_device_tree_on_RAM__ =  __secondary_RAM_vector_table_start_on_RAM__ + SECONDARY_RAM_VECTOR_AREA_SIZE ;

#if 0 //TODO : used for creating dynamic device tree file 
#ifdef CONFIG_DYNAMIC_DEVICE_TREE	
	__actual_start_of_device_tree_on_ROM__ = CONFIG_DEVICE_TREE_LOCATION_ADDR ;
#else
	__actual_start_of_device_tree_on_ROM__ = __start_of_device_tree_on_ROM__ ;
#endif
#endif
	. = __start_of_device_tree_on_RAM__;
	.start_of_device_tree . : AT ( __start_of_device_tree_on_ROM__ )
	{
		KEEP(*(.start_of_device_tree)) ;
	}  > DATA_LOCATION
	__static_devs_section_start_on_ROM__ = __start_of_device_tree_on_ROM__ + SIZEOF(.start_of_device_tree);
	.static_devs . : AT ( __static_devs_section_start_on_ROM__ )
	{
		KEEP(*(.static_devs)) ;
	}  > DATA_LOCATION
	__end_of_device_tree_on_ROM__ = __static_devs_section_start_on_ROM__ + SIZEOF(.static_devs);
	.end_of_device_tree . : AT ( __end_of_device_tree_on_ROM__ )
	{
		KEEP(*(.end_of_device_tree)) ;
	}  > DATA_LOCATION
	__static_devs_data_on_ROM__ = __end_of_device_tree_on_ROM__ + SIZEOF(.end_of_device_tree);
	.static_devs_data . : AT ( __static_devs_data_on_ROM__)
	{
		*(.static_devs_data) ;
		. = ALIGN(4);
	}  > DATA_LOCATION
	__end_of_static_devs_data_on_RAM__ = . ;
	
	/******end of device tree **********/
	/***********************************/

#ifdef CONFIG_DYNAMIC_DEVICE_TREE	
	__tables_relocation_section_end_on_ROM__ = __start_of_device_tree_on_ROM__ + CONFIG_DEVICE_TREE_MAXIMAL_SIZE ;
	__tables_relocation_section_end_on_RAM__ = __start_of_device_tree_on_RAM__ + CONFIG_DEVICE_TREE_MAXIMAL_SIZE ;
#else
	__tables_relocation_section_end_on_ROM__ = __static_devs_data_on_ROM__ + SIZEOF(.static_devs_data) ;
	__tables_relocation_section_end_on_RAM__ = . ;
#endif
	
	. = __tables_relocation_section_end_on_ROM__ ;

	.secondary_rom_vector_table . :
	{ 
	    KEEP(*(._secondary_rom_vector_table))
		. = ALIGN(4);
	} > CODE_LOCATION
	
	.text . :
	{ 
	   *(EXCLUDE_FILE ( FILES_TO_FORCE_IN_RAM ) .text )
	   *(EXCLUDE_FILE ( FILES_TO_FORCE_IN_RAM ) .text.*)
	}  > CODE_LOCATION

	. = ALIGN(4);
	.start_of_modules . :
	{ 
	   KEEP(*(.start_of_modules ))
	}  > CODE_LOCATION

	.modules . :
	{
	   KEEP(*(.modules ))
	}  > CODE_LOCATION

	.end_of_modules . :
	{ 
	   KEEP(*(.end_of_modules ))
	}  > CODE_LOCATION

	__auto_init_functions_section_start_on_RAM__ = .;
	.auto_init_functions . :
	{
		KEEP(*(.auto_init_functions)) ;
		. = ALIGN(4);
	}  > CODE_LOCATION
	__auto_init_functions_section_end_on_RAM__ = .;

	.rodata . :
	{
		*(EXCLUDE_FILE ( FILES_TO_FORCE_IN_RAM ) .rodata.*)
		*(EXCLUDE_FILE ( FILES_TO_FORCE_IN_RAM ) .rodata )
	}  > CODE_LOCATION

	. = ALIGN(4);
	.u_boot_list . :
	{
		KEEP(*(SORT(.u_boot_list*)));
	} > CODE_LOCATION

	.ARM.extab . : { *(.ARM.extab*) }  > CODE_LOCATION
	__exidx_start = .;
	.ARM.exidx . : { *(.ARM.exidx*) }  > CODE_LOCATION
	__exidx_end = .;

	. = ALIGN(4);
	__end_of_text__ = .;
	
	__relocation_section_start_on_ROM__ = __end_of_text__;
	__critical_text_section_start_on_ROM__ = __relocation_section_start_on_ROM__;
	
#ifdef CONFIG_CODE_LOCATION_INTERNAL_FLASH
	__relocation_section_start_on_RAM__ = __tables_relocation_section_end_on_RAM__ ;
#else
	__relocation_section_start_on_RAM__ = __end_of_text__ ;
#endif
	. = __relocation_section_start_on_RAM__ ;
	.critical_text . : AT ( __critical_text_section_start_on_ROM__ )
	{
		*(.critical_text);
		* (.text .text*);
		* (.rodata .rodata*);
		. = ALIGN(4);
	}  > DATA_LOCATION
	
	__data_section_start_on_ROM__ = __critical_text_section_start_on_ROM__ + SIZEOF(.critical_text);
	.data . : AT ( __data_section_start_on_ROM__ )
	{
		*(.data) ;
		*(.data.*) ;
		. = ALIGN(4);
	}  > DATA_LOCATION

	__relocation_section_end_on_RAM__ = .;


	
	.bss . :	
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*) 	*(COMMON)	
		. = ALIGN(4);
		__bss_end__ = .;
	}  > DATA_LOCATION
	
	.stack . : {
		. = ALIGN (8); /*cortex requirement*/
		Buttom_Of_Stacks = . ;
		
		
#ifdef CORTEX_M
		. += STACK_SIZE; 
#else /*cortex-a*/
		__irq_stack_buttom__ = . ;
		. += IRQ_STACK_SIZE; 
		. = ALIGN (8); 
		__irq_stack_top__ = . ;

		__fiq_stack_buttom__ = . ;
		. += FIQ_STACK_SIZE;
		. = ALIGN (8);
		__fiq_stack_top__ = . ;

		__svc_stack_buttom__ = . ;
		. += SVC_STACK_SIZE;
		. = ALIGN (8);
		__svc_stack_top__ = . ;

		__abt_stack_buttom__ = . ;
		. += ABT_STACK_SIZE; 
		. = ALIGN (8);
		__abt_stack_top__ = . ;

		__und_stack_buttom__ = . ;
		. += UND_STACK_SIZE;
		. = ALIGN (8);
		__und_stack_top__ = . ;

		__sys_stack_buttom__ = . ;
		. += SYS_STACK_SIZE;
		. = ALIGN (8);
		__sys_stack_top__ = . ;
#endif

		. = ALIGN (8); /*cortex requirement*/
		Top_Of_Stacks = .;
		Size_Of_Stack = Top_Of_Stacks - Buttom_Of_Stacks;
	} >DATA_LOCATION


	/* debug sections */
	INCLUDE DEBUG_SECTIONS_INCLUDE_FILE

		/* arm build attributes like cpu type , eabi type , etc. that was configured during build
	   can be added to text of code if this info is needed during run of program */
	.ARM.attributes 0   : {
	   __ARM.attributes_start = .;
	   *(.ARM.attributes*)
	   __ARM.attributes_end = .;
	}

    /* The following section is for checking illegal sections */
	 .dummy  : {  * (*) } > dummy_area


}

PROVIDE(__HEAP_START = Top_Of_Stacks );
PROVIDE(__END_OF_RAM =  ORIGIN(DATA_LOCATION) + LENGTH(DATA_LOCATION));
PROVIDE(__HEAP_END =   ORIGIN(DATA_LOCATION) + LENGTH(DATA_LOCATION));
