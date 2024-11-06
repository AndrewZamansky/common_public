/*
 * file : dt_interrupts_num.h
 *
 */

#ifndef dt_interrupts_num_h
#define dt_interrupts_num_h

#include "project_custom_interrupts_num.h"

#if (defined(CONFIG_CORTEX_M0) || defined(CONFIG_CORTEX_M3) \
		|| defined(CONFIG_CORTEX_M4))
	#if !defined(NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS)
		#error "NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS should be defined"
	#endif
#endif


#if (defined(CONFIG_CORTEX_A9) || defined(CONFIG_CORTEX_A35))
	#if !defined(NUMBER_OF_GIC_EXTERNAL_INTERRUPTS)
		#error "NUMBER_OF_GIC_EXTERNAL_INTERRUPTS should be defined"
	#endif
#endif

#endif /* dt_interrupts_num_h */
