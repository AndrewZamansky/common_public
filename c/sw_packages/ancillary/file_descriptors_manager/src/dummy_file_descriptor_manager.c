/******************************************************************************
 * @file     file_descriptor_manager.c
 * @brief
 *
 * @note
 *
 ******************************************************************************/

/*!<Includes */
#include "_project_typedefs.h"
#include "_project_defines.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errno.h"


struct stat {
	uint8_t dummy;
};

void _stat(const char *path, struct stat *buf)
{
	CRITICAL_ERROR("_stat not implemented yet");
}


int _write(int file, char *ptr, int len)
{
	CRITICAL_ERROR("_write not implemented yet");
	return len;
}


int _close(int file)
{
	CRITICAL_ERROR("_close not implemented yet");
	return -1;
}


int _fstat(int file, void *st)
{
	CRITICAL_ERROR("_fstat not implemented yet");
	return 0;
}


int _isatty(int file)
{
	CRITICAL_ERROR("_isatty not implemented yet");
	return 1;
}


int _lseek(int file, int ptr, int dir)
{
	CRITICAL_ERROR("_lseek not implemented yet");
	return 0;
}


int _open(const char *name, int flags, int mode)
{
	CRITICAL_ERROR("_open not implemented yet");
	return -1;
}


int _read(int file, char *ptr, int len)
{
	CRITICAL_ERROR("_read not implemented yet");
	return 0;
}
