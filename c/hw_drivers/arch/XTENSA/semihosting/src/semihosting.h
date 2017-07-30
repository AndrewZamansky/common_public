/*
 *
 * semihosting.h
 *
 *
 *
 *
 *
 */

#ifndef _SEMIHOSTING_H_
#define _SEMIHOSTING_H_

/********  includes *********************/


/********  types  *********************/

struct semihosting_instance_t {
	struct dev_desc_t *   callback_dev;
};

#endif
