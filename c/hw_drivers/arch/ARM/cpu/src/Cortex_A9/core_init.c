/* ****************************************
 *****************************************
	@file   core_init.c

	@date   :
	@project  :
	@description :
***************************************** */

/***************   includes    *******************/
#include "_project.h"

/***************   defines    *******************/




/***************   typedefs    *******************/



/**********   external variables    **************/


extern void do_startup(void);

/***********   loacal variables    **************/


/******************** default interrupt handlers ******************/



extern uint32_t __irq_stack_buttom__;
extern uint32_t __irq_stack_top__;
extern uint32_t __fiq_stack_buttom__;
extern uint32_t __fiq_stack_top__;
extern uint32_t __svc_stack_buttom__;
extern uint32_t __svc_stack_top__;
extern uint32_t __abt_stack_buttom__;
extern uint32_t __abt_stack_top__;
extern uint32_t __und_stack_buttom__;
extern uint32_t __und_stack_top__ ;
extern uint32_t __sys_stack_buttom__;
extern uint32_t __sys_stack_top__  ;

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


/******************-------------------*****************************/


/******************-------------------*****************************/


void fill_mem32_with_pattern(uint32_t *start , uint32_t *end , uint32_t pattern)
{
	// should be strong less (<) for not to overlap next stack area
	while (start < end)
	{
		*start++ = pattern;
	}
}

#if defined(CONFIG_CODE_LOCATION_INTERNAL_SRAM)
	#define VECTOR_TABLE_BA  0xfffd0000
#else
	#define VECTOR_TABLE_BA  0x0
#endif
void v7_outer_cache_inval_all(void);

/*
 *  function : low_level_init()
 */
void core_init(uint32_t curr_stack)
{

	/*
	 * assume that jump to  fill_mem32_with_pattern will
	 * not take more than 32 registers in stack
	 */
	curr_stack = (curr_stack & 0xffffffffc) - (4*32);


	v7_outer_cache_inval_all();

#if (1 == CONFIG_EXCEPTION_STACKS_DEBUG)
	fill_mem32_with_pattern(&__irq_stack_buttom__,
			&__irq_stack_top__ , 0xb1b1b1b1);
	fill_mem32_with_pattern(&__fiq_stack_buttom__,
			&__fiq_stack_top__ , 0xb2b2b2b2);
	fill_mem32_with_pattern(&__abt_stack_buttom__,
			&__abt_stack_top__ , 0xb4b4b4b4);
	fill_mem32_with_pattern(&__und_stack_buttom__,
			&__und_stack_top__ , 0xb5b5b5b5);
	fill_mem32_with_pattern(&__sys_stack_buttom__,
			&__sys_stack_top__ , 0xb6b6b6b6);
	fill_mem32_with_pattern(&__svc_stack_buttom__,
			&curr_stack , 0xb3b3b3b3);
#endif



	/* default exeption functions */
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x0)) = 0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x20)) = (uint32_t )do_startup;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x04)) = 0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x24)) =
							(uint32_t )do_undefine_instruction_exception;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x08)) = 0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x28)) =
									(uint32_t )do_software_interrupt_asm ;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x0c)) = 0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x2c)) =
									(uint32_t )do_prefetch_abort_exception;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x10)) = 0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x30)) =
										(uint32_t )do_data_abort_exception;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x14)) = 0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x34)) =
										(uint32_t )do_reserved_exception;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x18)) = 0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x38)) = (uint32_t )do_irq;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x1c)) = 0xE59FF018;
	*((volatile uint32_t *)(VECTOR_TABLE_BA + 0x3c)) = (uint32_t )do_fiq;
}
