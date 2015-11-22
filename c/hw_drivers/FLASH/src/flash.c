/*
 *
 *   file  :  FLASH.cpp
 *
 */



/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"

#include "FLASH_api.h"


/***************   defines    *******************/
#define FLASH_BASE_ADDRESS 0x40022000

#define FLASH_ACR (FLASH_BASE_ADDRESS + 0x0)
#define FLASH_ACR_LATENCY_F (0x00000007)
#define FLASH_ACR_LATENCY_ZERO_WAIT_STATE (0x00000000)
#define FLASH_ACR_LATENCY_ONE_WAIT_STATE (0x00000001)
#define FLASH_ACR_LATENCY_TWO_WAIT_STATE (0x00000002)

#define FLASH_KEYR (FLASH_BASE_ADDRESS + 0x4)
#define KEY1  0x45670123
#define KEY2  0xCDEF89AB

#define FLASH_SR (FLASH_BASE_ADDRESS + 0xC)
#define FLASH_SR_BSY_F 	(0x00000001)

#define FLASH_CR (FLASH_BASE_ADDRESS + 0x10)
#define FLASH_CR_PG_F 	(0x00000001)
#define FLASH_CR_PER_F 	(0x00000002)
#define FLASH_CR_STRT_F (0x00000040)
#define FLASH_CR_LOCK_F (0x00000080)

#define FLASH_AR (FLASH_BASE_ADDRESS + 0x14)

#define PAGE_SIZE  0x400



#define FLASH_HAL_NULL NULL

#define FLASH_hal_writeRegU32(addr,val)		( (*(volatile unsigned int *)(addr)) = (val) )
#define FLASH_hal_readRegU32(addr)			( *(volatile unsigned int *)(addr) )
#define FLASH_hal_writeRegU16(addr,val)		( (*(volatile unsigned short *)(addr)) = (val) )
#define FLASH_hal_readRegU16(addr)			( *(volatile unsigned short *)(addr) )

/***************   typedefs    *******************/


/**********   external variables    **************/



/***********   local variables    **************/

static uint32_t gLastProgramAddr;
static uint32_t gCurr_program_addr;

/*
 * function : FLASH_API_SetCorrectLatency()
 *
 *   should be called before setting new clock
 *
 */
uint32_t FLASH_API_SetCorrectLatency(uint32_t aClock_hz)
{
	uint32_t regVal;

	regVal=FLASH_hal_readRegU32(FLASH_ACR);
	regVal &= (~(FLASH_ACR_LATENCY_F));

	if(aClock_hz <= 24000000)
	{
		// do nothing
	}
	else if (aClock_hz <= 48000000)
	{
		regVal |= FLASH_ACR_LATENCY_ONE_WAIT_STATE;
	}
	else
	{
		regVal |= FLASH_ACR_LATENCY_TWO_WAIT_STATE;
	}

	FLASH_hal_writeRegU32(FLASH_ACR,regVal);

	return 0;
}


/*
 * function : FLASH_API_StartProgramming()
 *
 *
 *
 */
uint32_t FLASH_API_StartProgramming(uint32_t start_addr , uint32_t total_size_to_programm)
{
	uint32_t regVal;
	uint32_t current_addr;

	gLastProgramAddr	=	start_addr + total_size_to_programm ;
	gCurr_program_addr	=	start_addr ;
	// unlocking the flash
	FLASH_hal_writeRegU32(FLASH_KEYR,KEY1);
	FLASH_hal_writeRegU32(FLASH_KEYR,KEY2);

	//erase needed space
	current_addr=start_addr;
	regVal=FLASH_hal_readRegU32(FLASH_CR);
	regVal |= FLASH_CR_PER_F;
	FLASH_hal_writeRegU32(FLASH_CR,regVal);
	while(current_addr < gLastProgramAddr)
	{

		FLASH_hal_writeRegU32(FLASH_AR,current_addr);

		regVal |= FLASH_CR_STRT_F;
		FLASH_hal_writeRegU32(FLASH_CR,regVal);

		while( FLASH_hal_readRegU32(FLASH_SR) & FLASH_SR_BSY_F) ; // wait for action done

		current_addr += PAGE_SIZE;
	}

	regVal=FLASH_hal_readRegU32(FLASH_CR);
	regVal &= (~(FLASH_CR_PER_F));
	regVal |= FLASH_CR_PG_F;
	FLASH_hal_writeRegU32(FLASH_CR,regVal);

	return 0;
}

uint32_t FLASH_API_ContinueProgramming(uint8_t *buff,uint32_t size)
{
	uint32_t regVal;
	uint16_t newDataVal;

	if (0==size) return 0; // nothing to do


    if (gCurr_program_addr & 0x01) // if address is odd
    {
        newDataVal = FLASH_hal_readRegU16(gCurr_program_addr);
        newDataVal += ( buff[0] << 8 );
        FLASH_hal_writeRegU16(gCurr_program_addr,newDataVal);
        size--;
        gCurr_program_addr++;
    }

    while( size >1 )
    {
        newDataVal = buff[0];
        newDataVal += ( buff[1] << 8 );

        FLASH_hal_writeRegU16(gCurr_program_addr,newDataVal);

        while( FLASH_hal_readRegU32(FLASH_SR) & FLASH_SR_BSY_F) ; // wait for action done

        gCurr_program_addr +=2;
        size -= 2 ;
        buff += 2;
    }

    if ( size ) // if one byte left
    {
        newDataVal = buff[0];
        FLASH_hal_writeRegU16(gCurr_program_addr,newDataVal);

        while( FLASH_hal_readRegU32(FLASH_SR) & FLASH_SR_BSY_F) ; // wait for action done
        gCurr_program_addr++;
    }

    if(gCurr_program_addr >= gLastProgramAddr) // end of programming
    {
    	regVal=FLASH_hal_readRegU32(FLASH_CR);
		regVal &= (~(FLASH_CR_PG_F));
		regVal |= FLASH_CR_LOCK_F;
		FLASH_hal_writeRegU32(FLASH_CR,regVal);
    }

	return 0;
}


