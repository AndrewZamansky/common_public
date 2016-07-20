/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "version_management_api.h"



size_t version_management_pread(const void *aHandle ,uint8_t *apData , size_t aLength, size_t aOffset);


#define	STATIC_DEV_PREAD_FUNCTION	version_management_pread
#include "add_static_dev.h"
