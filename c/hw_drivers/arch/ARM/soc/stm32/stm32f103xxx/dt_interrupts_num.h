/*
 * file : dt_interrupts_num.h
 *
 */

#ifndef dt_interrupts_num_h
#define dt_interrupts_num_h

#if defined(STM32F10X_MD)
	#define NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS   43
#elif defined(STM32F10X_HD)
	#define NUMBER_OF_NVIC_EXTERNAL_INTERRUPTS   60
#else
	#error "unknown chip"
#endif

#endif /* */
