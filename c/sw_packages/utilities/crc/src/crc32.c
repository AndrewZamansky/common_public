/*
 *
 *   file  :  DevManagment.cpp
 *
 */



/***************   includes    *******************/
#include "crc_api.h" //place first to test that header file is self-contained

#include "crc_config.h"
#include "_crc_prerequirements_check.h" // should be after dev_managment_config.h



/***************   defines    *******************/

/***************   typedefs    *******************/



/**********   external variables    **************/

/***********   local variables    **************/




/*
 * The width of the CRC calculation and result.
 * Modify the typedef for a 16 or 32-bit CRC standard.
 */
typedef uint32_t crc_t;

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
	#define REFLECT_REMAINDER(X)	((crc_t) reflect((X), WIDTH))
#else
	#define REFLECT_REMAINDER(X)	(X)
#endif


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


crc_t calculate_crc32(uint8_t const message[], uint32_t nBytes)
{
	crc_t  remainder = 0xffffffff;
    uint32_t i;
    uint8_t bit,octet;


    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (i = 0; i < nBytes; i++)
    {
#if 1
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= (((crc_t)REFLECT_DATA(*message)) )<< (WIDTH - 8);

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
        	//divRes = remainder & TOPBIT;
            if (remainder & TOPBIT)
            {
                remainder = ( remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder =  remainder << 1;
            }
        }
#else
        octet = *message;
        for (i=0; i<8; i++)
        {
            if ((octet >> 7) ^ (remainder >> 31))
            {
            	remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
            	remainder = (remainder << 1);
            }
            octet <<= 1;
        }
#endif
    	message++;
    }
    /*
     * The final remainder is the CRC result.
     */
    return (REFLECT_REMAINDER(remainder) ^ 0xffffffff);

}   /* crcSlow() */

