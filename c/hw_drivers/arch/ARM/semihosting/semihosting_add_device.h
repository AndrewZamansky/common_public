/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "semihosting_api.h"
#include "src/semihosting.h"


#ifndef SEMIHOSTING_DT_CALLBACK_PDEV
	#define	__SEMIHOSTING_DT_CALLBACK_PDEV     NULL
#else
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(SEMIHOSTING_DT_CALLBACK_PDEV);
	#define __SEMIHOSTING_DT_CALLBACK_PDEV    \
				P_TO_STATIC_DEVICE_INST(SEMIHOSTING_DT_CALLBACK_PDEV)
#endif

SET_STATIC_DEV_CONFIG(semihosting) =
{
	__SEMIHOSTING_DT_CALLBACK_PDEV
};


#define MODULE_HAS_NO_RUNTIME_DATA

#include "add_static_dev.h"

/**
 *  device specific defines should be undefined
 *  after calling #include "add_static_dev.h"
 */
#undef SEMIHOSTING_DT_CALLBACK_PDEV
#undef __SEMIHOSTING_DT_CALLBACK_PDEV
