/*
 * file : SW_GPIO_WRAPPER.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _SW_GPIO_WRAPPER_H
#define _SW_GPIO_WRAPPER_H


/***************   typedefs    *******************/


typedef enum
{
	GPIO_API_OUT_CONTROLED_EXTERNALLY,
	GPIO_API_OUT_BLINK_LOW_SPEED,
	GPIO_API_OUT_BLINK_MEDIUM_SPEED,
	GPIO_API_OUT_BLINK_HIGH_SPEED

} GPIO_OutputBehavior_t;


typedef enum
{
	GPIO_Force_None=0,
	GPIO_Force_Set,
	GPIO_Force_Clear
} GPIO_Force_t;

typedef struct GPIO_Instance_t_{
	pdev_descriptor_t server_dev;
	GPIO_Force_t forceState;
	GPIO_OutputBehavior_t  outputBehavior;
	params_status_t paramStatus;
} GPIO_Instance_t;



#else
#pragma message( "warning : this header file had already been included" )
#endif /* */
