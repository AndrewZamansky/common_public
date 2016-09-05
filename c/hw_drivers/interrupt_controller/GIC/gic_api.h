
#ifndef _gic_API_H_
#define _gic_API_H_

/**********  define API  types ************/
#include "src/_gic_prerequirements_check.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt handler function prototype                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*GIC_Isr_T)(void);



/**********  define API  functions  ************/

void  GIC_API_Init(void);
void  GIC_API_Open(void);
int GIC_API_RegisterHandler(uint32_t  int_num, GIC_Isr_T func);
int GIC_API_EnableInt(uint32_t  int_num);
int GIC_API_DisableInt(uint32_t  int_num);
void GIC_API_EnableAllInt(void);
void GIC_API_DisableAllInt(void);
void  GIC_API_ExecuteIsr(uint32_t aIntNum);


#endif
