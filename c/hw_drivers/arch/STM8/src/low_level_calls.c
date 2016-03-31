/*
 * low_level_calls.c
 *
 *  Created on: Apr 20, 2015
 *      Author: azamansk
 */




@far @interrupt void NonHandledInterrupt (void)
{
	/* in order to detect unexpected events during development,
	   it is recommended to set a breakpoint on the following instruction
	*/
	return;
}

