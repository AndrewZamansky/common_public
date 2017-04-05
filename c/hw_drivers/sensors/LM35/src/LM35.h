/*
 * file : LM35.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _LM35_H
#define _LM35_H

typedef struct
{
	struct dev_desc_t const *adc_server_device;
} LM35_Instance_t;

typedef struct {
	uint8_t run_state ;
} LM35_runtime_instance_t;

#endif /* */
