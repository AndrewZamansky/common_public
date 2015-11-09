/*
 * file : dev_managment.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _dev_managment_H
#define _dev_managment_H

#include "dev_managment_config.h"
#include "_dev_managment_prerequirements_check.h" // should be after dev_managment_config.h

/***************   typedefs    *******************/

extern uint8_t get_dev_ioctl(uint8_t *dev_name_str, pdev_descriptor *dev_descriptor,
		uint8_t *dev_param_str, const dev_param_t **pIoctlParam);


#endif /* */
