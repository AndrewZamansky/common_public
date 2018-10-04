#include "project_config.h"
#include "dt_memory_layout.h"

ENTRY(_startup)

#include "arm_memory_blocks.lds"

STACK_SIZE = 1024;
EXCEPTION_VECTOR_AREA_SIZE  	= 0x200;

SECTIONS
{
#ifdef DATA_SECTION_LOCATION_INTERNAL_SRAM
	#define DATA_LOCATION internal_sram
	ram_start = ORIGIN(internal_sram);
#endif
#ifdef DATA_SECTION_LOCATION_EXTERNAL_DDR
	#define DATA_LOCATION ddr
	ram_start = ORIGIN(ddr);
#endif

#ifdef CODE_SECTION_LOCATION_INTERNAL_FLASH
	#define CODE_LOCATION internal_flash
	rom_start = ORIGIN(internal_flash);
	__tables_relocation_section_start_on_ROM__ =  rom_start ;
#endif

#ifdef CODE_SECTION_LOCATION_INTERNAL_SRAM
	#define CODE_LOCATION internal_sram
	__tables_relocation_section_start_on_ROM__ = ram_start ;
#endif

#ifdef CODE_SECTION_LOCATION_EXTERNAL_DDR
	#define CODE_LOCATION ddr
	__tables_relocation_section_start_on_ROM__ = ram_start ;
#endif

	__tables_relocation_section_start_on_RAM__ = ram_start;

	/* the AT process of first table is not used because it cause
	 * problem on nuc-link debugger to rom it : */
	/*.vector_table __tables_relocation_section_start_on_RAM__ : AT ( __tables_relocation_section_start_on_ROM__ ) */
	.vector_table __tables_relocation_section_start_on_ROM__ :
	{
		KEEP(*(._arm_vector_table))
	} > CODE_LOCATION

	.arm_additional_table . :
	{
		KEEP(*(._arm_additional_table*))
	} > CODE_LOCATION


	/********************************/
	/*******  device tree **********/
	start_of_device_tree_on_ROM =
		__tables_relocation_section_start_on_ROM__ + EXCEPTION_VECTOR_AREA_SIZE;
	start_of_device_tree_on_RAM =
		__tables_relocation_section_start_on_RAM__ + EXCEPTION_VECTOR_AREA_SIZE;

#if 0 //TODO : used for creating dynamic device tree file
#ifdef CONFIG_DYNAMIC_DEVICE_TREE
	__actual_start_of_device_tree_on_ROM__ = CONFIG_DEVICE_TREE_LOCATION_ADDR ;
#else
	__actual_start_of_device_tree_on_ROM__ = start_of_device_tree_on_ROM ;
#endif
#endif
	. = start_of_device_tree_on_RAM;

	__start_static_devs_section = .;
	static_devs_section . : AT ( start_of_device_tree_on_ROM )
	{
		KEEP(*(static_devs_section)) ;
	}  > DATA_LOCATION
	__stop_static_devs_section = .;

	static_devs_data_on_ROM =
			 start_of_device_tree_on_ROM + SIZEOF(static_devs_section);
	.static_devs_data . : AT ( static_devs_data_on_ROM)
	{
		*(.static_devs_data) ;
		. = ALIGN(4);
	}  > DATA_LOCATION
	__end_of_static_devs_data_on_RAM__ = . ;

	/******end of device tree **********/
	/***********************************/

#ifdef CONFIG_DYNAMIC_DEVICE_TREE
	tables_relocation_section_end_on_ROM =
		start_of_device_tree_on_ROM + CONFIG_DEVICE_TREE_MAXIMAL_SIZE ;
	__tables_relocation_section_end_on_RAM__ =
		start_of_device_tree_on_RAM + CONFIG_DEVICE_TREE_MAXIMAL_SIZE ;
#else
	tables_relocation_section_end_on_ROM =
		static_devs_data_on_ROM + SIZEOF(.static_devs_data) ;
	__tables_relocation_section_end_on_RAM__ = . ;
#endif

	. = tables_relocation_section_end_on_ROM ;

	.text . :
	{
	   *(EXCLUDE_FILE ( FILES_TO_FORCE_IN_RAM ) .text )
	   *(EXCLUDE_FILE ( FILES_TO_FORCE_IN_RAM ) .text.*)
	}  > CODE_LOCATION

	. = ALIGN(4);

	__start_modules_section = .;
	modules_section . :
	{
	   KEEP(*(modules_section))
	}  > CODE_LOCATION
	__stop_modules_section = .;


	__start_auto_init_section = .;
	auto_init_section . :
	{
		KEEP(*(auto_init_section)) ;
		. = ALIGN(4);
	}  > CODE_LOCATION
	__stop_auto_init_section = .;

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
	critical_text_section_start_on_ROM = __relocation_section_start_on_ROM__;

#ifdef CODE_SECTION_LOCATION_INTERNAL_FLASH
	__relocation_section_start_on_RAM__ = __tables_relocation_section_end_on_RAM__ ;
#else
	__relocation_section_start_on_RAM__ = __end_of_text__ ;
#endif
	. = __relocation_section_start_on_RAM__ ;
	.critical_text . : AT ( critical_text_section_start_on_ROM )
	{
		*(.critical_text);
		* (.text .text*);
		* (.rodata .rodata*);
		. = ALIGN(4);
	}  > DATA_LOCATION

	data_section_start_on_ROM =
			critical_text_section_start_on_ROM + SIZEOF(.critical_text);
	.data . : AT ( data_section_start_on_ROM )
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
		. += STACK_SIZE;
		. = ALIGN (8); /*cortex requirement*/
		Top_Of_Stacks = .;
		Size_Of_Stack = Top_Of_Stacks - Buttom_Of_Stacks;
	} >DATA_LOCATION


	/* debug sections */
	#include "arm_gcc_debug_sections.lds"

	/* arm build attributes like cpu type , eabi type ,
	 * etc. that was configured during build can be added to text
	 * of code if this info is needed during run of program */
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
