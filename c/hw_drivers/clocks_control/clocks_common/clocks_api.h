
#ifndef _clock_API_H_
#define _clock_API_H_

#include "src/_clocks_common_prerequirements_check.h"


#define CLOCK_INPUT_XTAL	0
#define CLOCK_CORE			1

/**********  define   types ************/
typedef uint32_t (*clock_get_func_t)(void )  ;
typedef uint8_t (*clock_set_func_t)(uint32_t )  ;

typedef struct
{
	uint32_t rate;
	clock_set_func_t clock_set_func;
	clock_get_func_t clock_get_func;
}clocks_common_t;


/**********  define    functions  ************/


/***** should be implemented in SOC  files  *************/

/********************************************************/
uint8_t clocks_api_add_clock(uint8_t clock_id , clocks_common_t *clock_params);
uint32_t clocks_api_get_rate(uint8_t clock_id );
uint8_t clocks_api_set_rate(uint8_t clock_id , uint32_t set_rate );



#endif
