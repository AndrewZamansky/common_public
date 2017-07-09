process to create ROM image with specific functions from pre-compiled libraries in GCC.

1) create file rom_function_list.c with create dummy constant array with list of required functions .
    for example if you want to use fabs from libm  (standard gcc library)   and  arm_biquad_cascade_df2T_f32 from
	larm_cortexM4lf_math (ARM CMSIS library)  the content of rom_function_list.c will be :
	/*--------------start----------------*/
    #include "math.h"
	#include "arm_math.h"

	typedef  void (*dummy_rom_func_t)(void)  ;

	const dummy_rom_func_t  rom_function_array[]=
	{
		(dummy_rom_func_t) arm_biquad_cascade_df2T_f32,
		(dummy_rom_func_t) fabs
	};
	/*--------------end----------------*/

	
2) compile 	rom_function_list.c and create rom_function_list.o .
   make sure to add proper include paths according to includes in rom_function_list.c  
   and make sure to compile with DEFINES that satisfy proper architecture , etc. 
   for previous example to compile for CORTEX-M with fpu support :
   $PATH_TO_GCC/arm-none-eabi-4.9.3/bin/arm-none-eabi-gcc   -mthumb -mcpu=cortex-m4 -mthumb-interwork  -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -g3 -ggdb3  -D__I= -D__IO= -DARM_MATH_CM4 -D__FPU_PRESENT=1 -I$PATH_TO_CMSIS\CMSIS\Include -I$PATH_TO_CMSIS\Device\ARM\ARMCM4\Include rom_function_list.c -c -o rom_function_list.o

3) create linker script file that described ROM location.
   for example for ROM on 0x80000 with size 0x40000 we will create file rom.lds :
   
	/*--------------start----------------*/
	MEMORY {
	 internal_rom : ORIGIN = 0x80000, LENGTH = 0x40000
	 dummy_area : ORIGIN = 0xFFFFFFFFFFFFFFFF, LENGTH = 0
	}

	SECTIONS
	{
	 __ROM_START__ = ORIGIN(internal_rom);
	 . = __ROM_START__;
	 .text . :
	 {
		*(.text*);
	 } > internal_rom
	 . = ALIGN(4);

	 .rodata . :
	 {
	  *(.rodata* )
	 } > internal_rom
	 .debug_frame    0  : { *(.debug*) }
	 .comment       0  : { *(.comment) }
	 .ARM.attributes 0 : { *(.ARM.attributes*) }
	 .dummy : { * (*) } > dummy_area
	}
	/*--------------end----------------*/
   
4) perform linkage of rom_function_list.o and libraries that include desired functions.
   make sure to add proper library paths .
   for previous example the linkage will be :
   $PATH_TO_GCC/arm-none-eabi-4.9.3/bin/arm-none-eabi-gcc   -mthumb -mcpu=cortex-m4 -mthumb-interwork  -nostartfiles   -Wl,-Map=rom.map   -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -g3 -ggdb3  -T rom.lds  rom_function_list.o -r  -larm_cortexM4lf_math -L$PATH_TO_CMSIS/CMSIS/Lib/GCC -lm -lc -o rom.elf


5) put rom.elf to ROM

6) compile firmware with ROM image as input .
   add rom.elf as input to linkage . 
   !! IMPORTANT !!  rom.elf should be placed BEFORE libraries in link command .
   for previous example the linkage will be :
   $PATH_TO_GCC/arm-none-eabi-4.9.3/bin/arm-none-eabi-gcc   -mthumb -mcpu=cortex-m4 -mthumb-interwork  -nostartfiles   -Wl,-Map=rom.map   -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -g3 -ggdb3  -T linker.lds  $OBJECTS_FILES -Wl,--just-symbols=rom.elf -larm_cortexM4lf_math -L$PATH_TO_CMSIS/CMSIS/Lib/GCC -lm -lc -lgcc -o out.elf
