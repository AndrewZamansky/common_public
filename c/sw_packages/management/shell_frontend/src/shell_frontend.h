/*
 * file : shell_frontend.h
 *
 *
 */

#ifndef _SHELL_FRONTEND_H
#define _SHELL_FRONTEND_H

#include "dev_management_api.h"

struct shell_frontend_runtime_instance_t {
	uint32_t lastTestedBytePos;
	uint8_t lastEOLchar;
};

SET_RUNTIME_DATA_TYPE(shell_frontend, struct shell_frontend_runtime_instance_t);

#endif
