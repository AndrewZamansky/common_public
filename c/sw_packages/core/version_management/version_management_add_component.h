/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "version_management_api.h"

size_t version_management_pread(struct dev_desc_t *adev ,uint8_t *apData , size_t aLength, size_t aOffset);

#define	MODULE_NAME				version_management
#define	MODULE_PREAD_FUNCTION	version_management_pread

#include "add_component.h"
