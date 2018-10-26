/*
 *
 * hw_crc_stm32f10x.c
 *
 *
 *
 *
 *
 */

/********  includes *********************/

#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "dev_management_api.h" // for device manager defines and typedefs


#include "stm32f10x_crc.h"
#include "stm32f10x_rcc.h"


/********  defines *********************/


/********  types  *********************/



/* ---------------------------- External variables ---------------------------------*/

/* ------------------------ External functions ------------------------------*/

/* ------------------------ Exported variables ------------------------------*/

#define POLYNOMIAL	0x04C11DB7
//#define WIDTH  (8 * sizeof(crc_t))
#define WIDTH  (32)
//#define TOPBIT (1 << (WIDTH - 1))
#define TOPBIT (0x80000000)

#define CONFIG_REFLECT_DATA	1
#define CONFIG_REFLECT_REMAINDER 1

#if (CONFIG_REFLECT_DATA == 1)
	#define REFLECT_DATA(X)			((uint8_t) reflect((X), 8))
#else
	#define REFLECT_DATA(X)			(X)
#endif

#if (CONFIG_REFLECT_REMAINDER == 1)
	#define REFLECT_REMAINDER(X)	((uint32_t) reflect((X), WIDTH))
#else
	#define REFLECT_REMAINDER(X)	(X)
#endif

/********  local defs *********************/


/*********************************************************************
 *
 * Function:    reflect()
 *
 * Description: Reorder the bits of a binary sequence, by reflecting
 *				them about the middle position.
 *
 * Notes:		No checking is done that nBits <= 32.
 *
 * Returns:		The reflection of the original data.
 *
 *********************************************************************/
static uint32_t reflect(uint32_t data, uint8_t nBits)
{
	uint32_t  reflection = 0x00000000;
	uint8_t  bit;

	/*
	 * Reflect the data about the center bit.
	 */
	for (bit = 0; bit < nBits; ++bit)
	{
		/*
		 * If the LSB bit is set, set the reflection of it.
		 */
		if (data & 0x01)
		{
			reflection |= (uint32_t)1 << ((nBits - 1) - bit);
		}

		data = (data >> 1);
	}

	return (reflection);

}	/* reflect() */



uint32_t  hw_crc_stm32f10x_api(const uint8_t *data , uint32_t length)
{
	uint32_t i;
	uint8_t j;

	if( 0 != (length & 0x3)) return 0; // can calculate only blocks alligned to uint32_t

	length = length >> 2;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE );
	CRC_ResetDR();
	for(i=0 ; i < length; i++)
	{
		uint32_t curr_data;

		curr_data=0;
		for(j = 0; j < 4; j++)
		{
			curr_data = curr_data << 8;
			curr_data |= REFLECT_DATA(data[j]) ;
		}
		CRC_AddDataToCrcCalculation(curr_data);
		data += 4;
	}

	return (REFLECT_REMAINDER( CRC_GetCRC()) ^ 0xffffffff);
}
