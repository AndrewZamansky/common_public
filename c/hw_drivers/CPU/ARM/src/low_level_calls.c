/* ****************************************
 *****************************************
	@file   low_level_calls.c

	@date   :
	@project  :
	@description :
***************************************** */

/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"


/***************   defines    *******************/




/***************   typedefs    *******************/



/**********   external variables    **************/
#define TEST_MAX_STACK_USAGE 1

extern uint32_t Buttom_Of_Stacks;
extern uint32_t Top_Of_Stacks;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __tables_relocation_section_start_on_RAM__;
extern uint32_t __tables_relocation_section_end_on_RAM__;
extern uint32_t __tables_relocation_section_start_on_ROM__;
extern uint32_t __relocation_section_start_on_RAM__;
extern uint32_t __relocation_section_end_on_RAM__;
extern uint32_t __relocation_section_start_on_ROM__;

extern void do_startup(void);
extern void do_software_interrupt_asm(void);
int smihosting_is_active = 0 ;
/***********   loacal variables    **************/


/******************** default interrupt handlers ******************/

void __attribute__((interrupt("SWI"))) do_app_software_interrupt(uint32_t int_num)
{
    //based on int_num, you can determine which system call is called
    switch(int_num)
    {
    	case 1:
    		break;
    	default:
    		break;

    }
}

#ifndef CORTEX_M

void __attribute__((interrupt("ABORT"))) do_data_abort_exception()
{
	while(1);
}

void __attribute__((interrupt("ABORT"))) do_prefetch_abort_exception()
{
	while(1);
}

void __attribute__((interrupt("IRQ"))) do_irq()
{

}

void __attribute__((interrupt("FIQ"))) do_fiq()
{
	while(1);
}

void __attribute__((interrupt("UNDEF"))) do_undefine_instruction_exception()
{
	while(1);
}

void do_reserved_exception()
{
	while(1);
}


#endif

void __attribute__((weak)) OS_SWI_Handler(void)
{

}

/******************-------------------*****************************/


#ifndef __DONT_USE_GCC_LIB

/**************  low level calls from libraries ******************/
int _getpid(void) {
  return 1;
}

void _kill(int pid)
{
	while(1) ;
}

extern void* __HEAP_START;
extern void* __HEAP_END;
unsigned char *gloabal_heap = NULL;
void* _sbrk ( int incr )
{
  unsigned char *prev_heap;

  if( ( gloabal_heap + incr) >= (unsigned char *)&__HEAP_END)
  {
	  while(1); // trap of memmory overflow
  }

  if (gloabal_heap == NULL) {
	  gloabal_heap = (unsigned char *)&__HEAP_START;
  }
  prev_heap = gloabal_heap;
  /* check removed to show basic approach */

  gloabal_heap += incr;

  return (void*) prev_heap;
}


void __attribute__ ((noreturn)) _exit(int status)
{
	while(1);
}

int _write(int file, char *ptr, int len) {

 return len;
 }

int _close(int file) { return -1; }

int _fstat(int file, void *st)
{
 return 0;
}

int _isatty(int file) { return 1; }

int _lseek(int file, int ptr, int dir) { return 0; }

int _open(const char *name, int flags, int mode) { return -1; }

int _read(int file, char *ptr, int len) {
  return 0;

}
#endif
/******************-------------------*****************************/




#ifdef REMAP_EXEPTION_TABLE_TO_ROM
	#define VECTOR_TABLE_BA  0xfffd0000
#else
	#define VECTOR_TABLE_BA  0x0
#endif
void v7_outer_cache_inval_all(void);

/*
 *  function : low_level_init()
 */
void low_level_init()
{
	uint32_t *src ;
	uint32_t *dst ;

#ifdef CORTEX_M

#else /* cortex - a9 */

	v7_outer_cache_inval_all();

#ifdef NUVOTON_POLEG
	#ifdef REMAP_EXEPTION_TABLE_TO_ROM
	/* remap 0x100 bytes of 0xffff0000 to sram  */
		*((volatile uint32_t *)0xf0800074) |=  (1<<18);
	#else
		*((volatile uint32_t *)0xf0800074) &=  (~(1<<18));
	#endif
#endif

	/* default exeption functions */
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x0))=0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x20))=(uint32_t )do_startup;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x04))=0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x24))=(uint32_t )do_undefine_instruction_exception;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x08))=0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x28))=(uint32_t )do_software_interrupt_asm ;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x0c))=0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x2c))=(uint32_t )do_prefetch_abort_exception;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x10))=0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x30))=(uint32_t )do_data_abort_exception;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x14))=0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x34))=(uint32_t )do_reserved_exception;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x18))=0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x38))=(uint32_t )do_irq;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x1c))=0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x3c))=(uint32_t )do_fiq;
#endif
	/* ROM has tables that needs to be copied to RAM. */
	src=&__tables_relocation_section_start_on_ROM__;
	dst=&__tables_relocation_section_start_on_RAM__;
	if(src!=dst)
	{
		while (dst < &__tables_relocation_section_end_on_RAM__)
		{
		  *dst++ = *src++; /* 4 bytes copy*/
		}
	}

	/* ROM has code and data that needs to be copied to RAM. */
	src=&__relocation_section_start_on_ROM__;
	dst=&__relocation_section_start_on_RAM__;
	if(src!=dst)
	{
		while (dst < &__relocation_section_end_on_RAM__)
		{
		  *dst++ = *src++; /* 4 bytes copy*/
		}
	}

	/* Zero bss */
	for (dst = &__bss_start__; dst< &__bss_end__; dst++ /* 4 bytes copy*/)
	  *dst = 0;

#if (1==TEST_MAX_STACK_USAGE)
	for (dst = &Buttom_Of_Stacks; dst<= (&Top_Of_Stacks-100); dst++ /* 4 bytes copy ,  -100 for not */)
	  *dst = 0xb6b6b6b6;
#endif
}
