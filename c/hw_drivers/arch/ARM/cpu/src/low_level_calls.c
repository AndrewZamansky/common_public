/* ****************************************
 *****************************************
	@file   low_level_calls.c

	@date   :
	@project  :
	@description :
***************************************** */

/***************   includes    *******************/
#include "_project.h"

/***************   defines    *******************/




/***************   typedefs    *******************/



/**********   external variables    **************/




/***********   loacal variables    **************/


/******************** default interrupt handlers ******************/




/******************-------------------*****************************/



/**************  low level calls from libraries ******************/
EXTERN_C_FUNCTION int _getpid(void) {
	while(1) ;
	return 1;
}

EXTERN_C_FUNCTION void _kill(int pid)
{
	while(1) ;
}

extern void* __HEAP_START;
extern void* __HEAP_END;
static unsigned char *global_heap = NULL;
EXTERN_C_FUNCTION void* _sbrk ( int incr )
{
	unsigned char *prev_heap;

	if (global_heap == NULL)
	{
		global_heap = (unsigned char *)&__HEAP_START;
	}

	if( ( global_heap + incr) > (unsigned char *)&__HEAP_END)
	{
		__asm__ __volatile__("cpsid i\n");
		while(1); // trap of memmory overflow
	}

	prev_heap = global_heap;
	/* check removed to show basic approach */

	global_heap += incr;

	return (void*) prev_heap;
}


size_t get_heap_left()
{
	return (((unsigned char *)&__HEAP_END) - global_heap);
}


EXTERN_C_FUNCTION void __attribute__ ((noreturn)) _exit(int status)
{
	while(1);
}

EXTERN_C_FUNCTION clock_t _times(clock_t * ptms) { while(1) ;return -1; }
