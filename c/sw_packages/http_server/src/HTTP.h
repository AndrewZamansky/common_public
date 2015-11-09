/*
 * file : HTTP.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _HTTP_H
#define _HTTP_H


/***************   typedefs    *******************/

typedef struct
{
	void *low_levelSocketInstance;
	uint8_t unused_bytes_left;
	char *requestStr;
} HTTP_Socket_Instance_t;


#else
#pragma message( "warning : this header file had already been included" )
#endif /* */
