
ENTRY(_secondary_rom_vector_table_startup_entry)

MEMORY {
	flash	:   ORIGIN = CONFIG_FLASH_START_ADDR, 	LENGTH = CONFIG_FLASH_SIZE    
	ram  	:   ORIGIN = CONFIG_RAM_START_ADDR ,   	LENGTH =  CONFIG_RAM_SIZE  
    
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
	__RAM_START__ = ORIGIN(ram);
	__FLASH_START__ = ORIGIN(flash);
	

#ifdef RUN_FROM_FLASH
	#define CODE_LOCATION flash
	__tables_relocation_section_start_on_ROM__ =  __FLASH_START__ ;
#else
	#define CODE_LOCATION ram
	__tables_relocation_section_start_on_ROM__ = __RAM_START__ ;
#endif	
	__tables_relocation_section_start_on_RAM__ = __RAM_START__;

	/* the AT process of first table is not used because it cause problem on nuc-link debugger to flash it : */
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
	} > ram

	__tables_relocation_section_end_on_ROM__ = __secondary_RAM_vector_table_start_on_ROM__ + SIZEOF(.secondary_ram_vector_table) ;
	__tables_relocation_section_end_on_RAM__ = __secondary_RAM_vector_table_start_on_RAM__ + SIZEOF(.secondary_ram_vector_table) ;

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
	
#ifdef RUN_FROM_FLASH
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
 	}  > ram
	
 	__data_section_start_on_ROM__ = __critical_text_section_start_on_ROM__ + SIZEOF(.critical_text);
 	.data . : AT ( __data_section_start_on_ROM__ ) 
 	{ 
 		*(.data) ;
 		*(.data.*) ;
		. = ALIGN(4);
 	}  > ram
 	
	__relocation_section_end_on_RAM__ = .;


	
	.bss . :	
	{ 
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*) 	*(COMMON)	
		. = ALIGN(4);
		__bss_end__ = .;
	}  > ram
	
	.stack . : { 
		. = ALIGN (8); /*cortex requirement*/
		Buttom_Of_Stacks = . ; 
		
		
#ifdef CORTEX_M
		. += STACK_SIZE; 
#else /*cortex-a*/
		. += IRQ_STACK_SIZE; 
		. = ALIGN (8); 
		__irq_stack_top__ = . ; 

		. += FIQ_STACK_SIZE; 
		. = ALIGN (8); 
		__fiq_stack_top__ = . ; 

		. += SVC_STACK_SIZE; 
		. = ALIGN (8); 
		__svc_stack_top__ = . ; 

		. += ABT_STACK_SIZE; 
		. = ALIGN (8);
		__abt_stack_top__ = . ; 

		. += UND_STACK_SIZE; 
		. = ALIGN (8); 
		__und_stack_top__ = . ; 

		. += SYS_STACK_SIZE; 
		. = ALIGN (8); 
		__sys_stack_top__ = . ; 
#endif

		. = ALIGN (8); /*cortex requirement*/
		Top_Of_Stacks = .; 
		Size_Of_Stack = Top_Of_Stacks - Buttom_Of_Stacks;
	} >ram 


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
PROVIDE(__END_OF_RAM =  ORIGIN(ram) + LENGTH(ram));
PROVIDE(__HEAP_END =   ORIGIN(ram) + LENGTH(ram));
