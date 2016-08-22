/*
 * file : shell_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _shell_prerequirements_check_H
#define _shell_prerequirements_check_H

#include "_project.h"
#include "dev_management_api.h"
#include "os_wrapper.h"
#include "global_prerequirements_check.h"


/***************************/
/******** test defines *****/




#ifndef SHELL_TASK_PRIORITY
    #error "SHELL_TASK_PRIORITY should be defined in project header files"
#endif

#ifndef SHELL_TASK_STACK_SIZE
    #error "SHELL_TASK_PRIORITY should be defined in project header files"
#endif

#ifndef CONFIG_SHELL_MAX_QUEUE_LEN
    #error "CONFIG_SHELL_MAX_QUEUE_LEN should be defined in project header files"
#endif

/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/



/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/




/***************************/




#endif /* */
