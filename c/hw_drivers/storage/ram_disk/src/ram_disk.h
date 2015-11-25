/*
 * file : BuiltInFlash.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _BuiltInFlash_H
#define _BuiltInFlash_H


/***************   typedefs    *******************/


#define SECTORS_PER_TRACK			1 //
#define NUM_OF_CYLINDERS			0x11 // windows will not recognize less then 0x11
#define NUM_OF_HEADS				0x1 //

#define REPORTED_BLOCKS_ON_STORAGE	   ((NUM_OF_HEADS * NUM_OF_CYLINDERS) * SECTORS_PER_TRACK)
#define REPORTED_BLOCKS_ON_PARTITION   (REPORTED_BLOCKS_ON_STORAGE -1) // first block is MBR
#define MASS_STORAGE_MEMORY_BLOCK_SIZE	512

#define REAL_MASS_STORAGE_MEMORY_BLOCKS			8
#define REAL_MASS_STORAGE_MEMORY_SIZE	(REAL_MASS_STORAGE_MEMORY_BLOCKS*MASS_STORAGE_MEMORY_BLOCK_SIZE)

typedef struct {
	BuiltInFlash_API_Chip_t chip;
} BuiltInFlash_Instance_t;


#else
#pragma message( "warning : this header file had already been included" )
#endif /* */
