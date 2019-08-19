
#ifndef _irq_api_H_
#define _irq_api_H_


#include "cpu_config.h"
#include "dev_management_api.h"
#include "_project_interrupt_priorities.h"




typedef void (*isr_t)(void)  ;

int irq_register_interrupt(int int_num , isr_t pIsr);

void irq_enable_interrupt(int int_num);

void irq_disable_interrupt(int int_num);

void irq_block_all();
void irq_unblock_all();

/**********  define API  functions  ************/

void  GIC_API_Init(void);
void  GIC_API_Open(void);
void  GIC_API_ExecuteIsr(uint32_t aIntNum);


#endif
