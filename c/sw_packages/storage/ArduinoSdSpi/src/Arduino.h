/*
 * file : ArduinoSdSpi.h
 *
 *
 */

#ifndef _Arduino_H
#define _Arduino_H

#include "dev_management_api.h"


#define HIGH 0x01
#define LOW 0x00
#define OUTPUT 0
/***************   typedefs    *******************/

struct arduino_instance_t{
	struct dev_desc_t * server_dev;
};


#endif
