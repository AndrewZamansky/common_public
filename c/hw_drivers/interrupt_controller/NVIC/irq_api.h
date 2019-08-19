
#ifndef _irq_api_H_
#define _irq_api_H_


#include "cpu_config.h"
#include "dev_management_api.h"
#include "_project_interrupt_priorities.h"


#if !defined(MAX_INTERRUPT_PRIO_FOR_OS_SYSCALLS)
	#error "MAX_INTERRUPT_PRIO_FOR_OS_SYSCALLS should be defined "
#endif

#define CHECK_INTERRUPT_PRIO_FOR_OS_SYSCALLS(l)    \
						(MAX_INTERRUPT_PRIO_FOR_OS_SYSCALLS > l)


#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
  #define IRQ_ATTR  __attribute__((interrupt("IRQ")))
#elif defined(__ARMCC_VERSION)
  #define IRQ_ATTR  __irq
#endif


typedef IRQ_ATTR void (*isr_t)(void);

/**
 * irq_register_interrupt()
 * @int_num  : interrupt number RELETIVE to -16 .
 *             for example if UART interrupt is next after sysTick interrupt
 *             then   int_num = 0 ( and not 16)
 *
 * return:
 */
int irq_register_interrupt(int int_num , isr_t pIsr);


/**
 * irq_register_device_on_interrupt()
 * @int_num  : interrupt number RELETIVE to -16 .
 *             for example if UART interrupt is next after sysTick interrupt
 *             then   int_num = 0 ( and not 16)
 *
 * return:
 */
int irq_register_device_on_interrupt(int int_num , struct dev_desc_t * pdev);


/**
 * irq_enable_interrupt()
 * @int_num  : interrupt number RELETIVE to -16 .
 *             for example if UART interrupt is next after sysTick interrupt
 *             then   int_num = 0 ( and not 16)
 *
 * return:
 */
#define irq_enable_interrupt(int_num)  		NVIC_EnableIRQ(int_num)


/**
 * irq_disable_interrupt()
 * @int_num  : interrupt number RELETIVE to -16 .
 *             for example if UART interrupt is next after sysTick interrupt
 *             then   int_num = 0 ( and not 16)
 *
 * return:
 */
int irq_disable_interrupt(int int_num);



/***************** unblock interrupts reception **************/
#define irq_unblock_all()	__asm__ __volatile__("cpsie i\n");

/***************** block interrupts reception **************/
#define irq_block_all()		__asm__ __volatile__("cpsid i\n");


/**
 * irq_set_priority()
 * @int_num  : interrupt number RELETIVE to -16 .
 *             for example if UART interrupt is next after sysTick interrupt
 *             then   int_num = 0 ( and not 16)
 *
 * return:
 */
#define irq_set_priority(int_num, priority) NVIC_SetPriority(int_num, priority)



#endif
