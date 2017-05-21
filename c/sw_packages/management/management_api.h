
#ifndef _management_api_h_
#define _management_api_h_

#include "dev_management_api.h" // for device manager defines and typedefs

/*****************  defines  **************/


/**********  define API  types ************/


/**********  define API  functions  ************/
struct rcvd_cmd_t {
	struct dev_desc_t * reply_dev;
	uint8_t  *cmd_buf;
	size_t	cmd_len;
};

#endif
