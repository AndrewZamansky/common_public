/* !! DONT PUT HEADER FILE PROTECTIONS IN THIS FILE !! */

#include  "clock_control_api.h"
#include  "clock_control_common/src/clock_control_common.h"


#ifndef CLK_DT_DEFAULT_PARENT
	#define CLK_DEFAULT_PARENT     NULL
#else
	EXTERN_DECLARATION_TO_STATIC_DEVICE_INST(CLK_DT_DEFAULT_PARENT) ;
	#define CLK_DEFAULT_PARENT   P_TO_STATIC_DEVICE_INST(CLK_DT_DEFAULT_PARENT)
#endif

#ifndef CLK_DT_SET_FREQ_FUNC
	#define  CLK_DT_SET_FREQ_FUNC     NULL
#endif

#ifndef CLK_DT_GET_FREQ_FUNC
	#define  CLK_DT_GET_FREQ_FUNC     NULL
#endif

#ifndef CLK_DT_SET_PARENT_CLK_FUNC
	#define  CLK_DT_SET_PARENT_CLK_FUNC     NULL
#endif

#ifndef CLK_DT_ENABLE_CLK_FUNC
	#define  CLK_DT_ENABLE_CLK_FUNC     NULL
#endif

SET_STATIC_DEV_CONFIG(clk_cntl) =
{
	CLK_DEFAULT_PARENT,
	CLK_DT_GET_FREQ_FUNC,
	CLK_DT_SET_FREQ_FUNC,
	CLK_DT_SET_PARENT_CLK_FUNC,
	CLK_DT_ENABLE_CLK_FUNC
};


#include "add_static_dev.h"

/*
 * device specific defines should be undefined after
 *  calling #include "add_static_dev.h"
 */
#undef CLK_DEFAULT_PARENT
#undef CLK_DT_DEFAULT_RATE
#undef CLK_DT_DEFAULT_PARENT
#undef CLK_DT_ENABLE_CLK_FUNC
#undef CLK_DT_SET_PARENT_CLK_FUNC
#undef CLK_DT_SET_FREQ_FUNC
#undef CLK_DT_GET_FREQ_FUNC
