/*
 * file : freeRtos_cortex_M_port.c
 *
 *
 *
 *
 *
 *
 */

#include "irq_api.h"


extern void xPortPendSVHandler(void);
extern void vPortSVCHandler(void);

void os_start_arch_related_components()
{
	irq_register_interrupt(CONFIG_DT_SV_CALL_INTERRUPT , vPortSVCHandler);
	irq_register_interrupt(CONFIG_DT_PEND_SV_INTERRUPT , xPortPendSVHandler);

}
