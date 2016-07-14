/*
 * file : dev_management.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _dev_management_H
#define _dev_management_H

#include "_dev_management_prerequirements_check.h" // should be after dev_management_config.h

/***************   typedefs    *******************/

extern uint8_t get_dev_ioctl(uint8_t *dev_name_str, pdev_descriptor_t *dev_descriptor,
		uint8_t *dev_param_str, const dev_param_t **pIoctlParam);


#endif /* */
