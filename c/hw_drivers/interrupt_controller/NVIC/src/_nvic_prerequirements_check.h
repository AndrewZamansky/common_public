/*
 * file : nvic_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _nvic_prerequirements_check_H
#define _nvic_prerequirements_check_H


#include "_project.h"
#include "dev_management_api.h"
#include "cpu_config.h"
#include "global_prerequirements_check.h"

/***************************/
/******** test defines *****/


#ifndef TOTAL_NUMBER_OF_EXTERNAL_INTERRUPS
	#error  "TOTAL_NUMBER_OF_EXTERNAL_INTERRUPS should be defined in cpu description"
#endif

/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/



/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/


/***************************/


#endif /* */
