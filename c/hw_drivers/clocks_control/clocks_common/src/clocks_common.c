/*
 *
 *   file  : clocks_common.c
 *
 */



/***************   includes    *******************/
#include "src/_clocks_common_prerequirements_check.h"

#include "clocks_api.h"


/***************   defines    *******************/



/***************   typedefs    *******************/

typedef struct
{
	uint8_t clock_id;
	clocks_common_t *clock_params;
}clocks_t;

/**********   external variables    **************/



/***********   local variables    **************/


static clocks_t clocks_array[CONFIG_MAX_NUMBER_OF_CLOCKS] = {{0}};

static uint16_t usedClocks = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        clocks_api_set_rate                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t clocks_api_set_rate(uint8_t clock_id ,  uint32_t set_rate)
{
	uint32_t i;
	clocks_t *curr_clock ;
	clocks_common_t *clock_params ;

	curr_clock = clocks_array;
	for(i=0 ; i<CONFIG_MAX_NUMBER_OF_CLOCKS ; i++)
	{
		if( clock_id == curr_clock->clock_id)
		{
			clock_params = curr_clock->clock_params;
			if (NULL != clock_params)
			{
				return clock_params->clock_set_func(set_rate);
			}
			else
			{
				return 1;
			}
		}
	}

	return 1;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        clocks_api_get_rate                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t clocks_api_get_rate(uint8_t clock_id )
{
	uint32_t i;
	clocks_t *curr_clock ;
	clocks_common_t *clock_params ;

	curr_clock = clocks_array;
	for(i=0 ; i<CONFIG_MAX_NUMBER_OF_CLOCKS ; i++)
	{
		if( clock_id == curr_clock->clock_id)
		{
			clock_params = curr_clock->clock_params;
			if (NULL != clock_params)
			{
				if (0 != clock_params->rate)
				{
					return clock_params->rate;
				}
				return clock_params->clock_get_func();
			}
			else
			{
				return 0;
			}
		}
	}

	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        add_clock_common                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t clocks_api_add_clock(uint8_t clock_id , clocks_common_t *clock_params)
{
	if(CONFIG_MAX_NUMBER_OF_CLOCKS <= usedClocks)
	{
		return 1;
	}

	clocks_array[usedClocks].clock_id=clock_id;
	clocks_array[usedClocks].clock_params=clock_params;


	usedClocks++;
	return 0;
}
