/*
 * file : DELAY.h
 *
 */

#ifndef _DELAY_H
#define _DELAY_H


struct delay_instance_t {
	size_t delay_samples;
	real_t  *buff;
};

#endif
