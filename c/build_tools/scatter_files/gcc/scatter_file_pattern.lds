
ENTRY(_inner_vector_table)

MEMORY {
	flash	:   ORIGIN = CONFIG_FLASH_START_ADDR, 	LENGTH = CONFIG_FLASH_SIZE    
	ram  	:   ORIGIN = CONFIG_RAM_START_ADDR ,   	LENGTH =  CONFIG_RAM_SIZE  
    
    /* The following section is for checking illegal sections */
    dummy_area    :   ORIGIN = 0xFFFFFFFFFFFFFFFF,           LENGTH = 0   
}

#ifdef CORTEX_M
	STACK_SIZE = 1024;
	EXCEPTION_VECTOR_AREA_SIZE  = 0x130;
#else /*cortex-a*/
	SYS_STACK_SIZE = 128;
	IRQ_STACK_SIZE = 1024; 
	FIQ_STACK_SIZE = 128; 
	SVC_STACK_SIZE = 1024; 
	ABT_STACK_SIZE = 128; 
	UND_STACK_SIZE = 128; 
	EXCEPTION_VECTOR_AREA_SIZE  = 0x100;
#endif

SECTIONS
{
	__RAM_START__ = ORIGIN(ram);
	__FLASH_START__ = ORIGIN(flash);
	
#ifdef RUN_FROM_FLASH
	#define CODE_LOCATION flash
	#define CODE_START_ADDR  __FLASH_START__
#else
	#define CODE_LOCATION ram
	#define CODE_START_ADDR  __RAM_START__
#endif	

	. = CODE_START_ADDR;
	.vectors : { KEEP(*(._arm_vector_table))  }  > CODE_LOCATION /*vectors are prpgrammable in RAM */
	. = CODE_START_ADDR + EXCEPTION_VECTOR_AREA_SIZE ;
	__APP_START_ADDR__ = . ;
	.startup . : { KEEP(*(._inner_vector_table)) } > CODE_LOCATION
	
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

#ifdef RUN_FROM_FLASH
	. = __RAM_START__ + EXCEPTION_VECTOR_AREA_SIZE ;
#endif

	__critical_text_section_start_on_rom__ = __end_of_text__;
 	.critical_text . : AT ( __critical_text_section_start_on_rom__ ) 
 	{ 
	 	. = ALIGN(4);
		__critical_text_section_start__ = .;
 		*(.critical_text);
		* (.text .text*);
		* (.rodata .rodata*);
		. = ALIGN(4);
		__critical_text_section_end__ = .;	
 	}  > ram
	
 	__data_section_start_on_rom__ = __end_of_text__ + SIZEOF(.critical_text);
 	.data . : AT ( __data_section_start_on_rom__ ) 
 	{ 
	 	. = ALIGN(4);
		__data_section_start__ = .;

 		*(.data) ;
 		*(.data.*) ;
		. = ALIGN(4);
		__data_section_end__ = .;
 	}  > ram
 	

	
	.bss . :	
	{ 
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*) 	*(COMMON)	
		. = ALIGN(4);
		__bss_end__ = .;
	}  > ram
	
	.stack . : { 
		. = ALIGN (8); 
		Buttom_Of_Stacks = . ; 
		
#ifdef CORTEX_M
		. += STACK_SIZE; 
#else /*cortex-a*/
		. += IRQ_STACK_SIZE; 
		. = ALIGN (4); 
		__irq_stack_top__ = . ; 

		. += FIQ_STACK_SIZE; 
		. = ALIGN (4); 
		__fiq_stack_top__ = . ; 

		. += SVC_STACK_SIZE; 
		. = ALIGN (4); 
		__svc_stack_top__ = . ; 

		. += ABT_STACK_SIZE; 
		. = ALIGN (4);
		__abt_stack_top__ = . ; 

		. += UND_STACK_SIZE; 
		. = ALIGN (4); 
		__und_stack_top__ = . ; 

		. += SYS_STACK_SIZE; 
		. = ALIGN (4); 
		__sys_stack_top__ = . ; 
#endif

		Top_Of_Stacks = .; 
		Size_Of_Stack = Top_Of_Stacks - Buttom_Of_Stacks;
	} >ram 

	_end =. + 4;

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
 	
	__APP_START_ON_FLASH_ADDR__ = __FLASH_START__ ;
	__APP_ENTRY_ADDR__ = __FLASH_START__ + EXCEPTION_VECTOR_AREA_SIZE;
	
}

PROVIDE(__HEAP_START = _end );
PROVIDE(__END_OF_RAM =  ORIGIN(ram) + LENGTH(ram));
PROVIDE(__HEAP_END =   ORIGIN(ram) + LENGTH(ram));
