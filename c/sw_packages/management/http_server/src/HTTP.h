/*
 * file : HTTP.h
 *
 */

#ifndef _HTTP_H
#define _HTTP_H


struct HTTP_Socket_Instance_t {
	void *low_levelSocketInstance;
	uint8_t unused_bytes_left;
	char *requestStr;
};


#endif
