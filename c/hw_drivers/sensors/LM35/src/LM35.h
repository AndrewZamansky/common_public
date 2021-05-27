/*
 * file : LM35.h
 *
 *
 */

#ifndef _LM35_H
#define _LM35_H

struct LM35_Instance_t {
	struct dev_desc_t const *adc_server_device;
};

struct LM35_runtime_instance_t {
	uint8_t run_state ;
};

#endif
