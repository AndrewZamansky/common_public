
#ifndef _nvic_API_H_
#define _nvic_API_H_

#include "nvic_config.h"

/*****************  defines  **************/

#define NVIC_API_EnableInt(int_num)  NVIC_EnableIRQ(int_num)
#define NVIC_API_SetPriority(int_num,priority)  NVIC_SetPriority(int_num,priority)
#define NVIC_API_EndOfService(int_num)  NVIC_ClearPendingIRQ(int_num)

/**********  define API  types ************/


typedef void (*NVIC_Isr_t)(void)  ;


/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt handler function prototype                                                                    */
/*---------------------------------------------------------------------------------------------------------*/



/**********  define API  functions  ************/
void  NVIC_API_Init(void);
void  NVIC_API_Start(void);
int NVIC_API_RegisterInt(IRQn_Type int_num , NVIC_Isr_t pIsr);
int NVIC_API_DisableInt(IRQn_Type int_num);
void NVIC_API_EnableAllInt(void);
void NVIC_API_DisableAllInt(void);

#endif
