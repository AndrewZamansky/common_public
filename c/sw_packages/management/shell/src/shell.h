/*
 * file : shell.h
 *
 *
 */

#ifndef _SHELL_H
#define _SHELL_H

#include "dev_management_api.h"


/***************   typedefs    *******************/



struct shell_runtime_instance_t {
	uint32_t lastTestedBytePos;
	uint8_t lastEOLchar;
};

#endif /* */
