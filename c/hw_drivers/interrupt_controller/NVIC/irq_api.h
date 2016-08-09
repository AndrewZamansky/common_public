
#ifndef _irq_api_H_
#define _irq_api_H_

#include "_project.h"
#include "src/_nvic_prerequirements_check.h"

#include "cpu_config.h"



typedef void (*isr_t)(void)  ;

/***************** register isr for interrupt **************/
int irq_register_interrupt(int int_num , isr_t pIsr);

/***************** register device on interrupt **************/
int irq_register_device_on_interrupt(int int_num , pdev_descriptor_t pdev);

/***************** enable interrupt **************/
#define irq_enable_interrupt(int_num)  		NVIC_EnableIRQ(int_num)

/***************** disable interrupt **************/
int irq_disable_interrupt(int int_num);

/***************** unblock interrupts reception **************/
#define irq_unblock_all()	__asm__ __volatile__("cpsie i\n");

/***************** block interrupts reception **************/
#define irq_block_all()		__asm__ __volatile__("cpsid i\n");


/***************** set interrupt priority **************/
#define irq_set_priority(int_num,priority)  NVIC_SetPriority(int_num,priority)



#endif
