/*
 * file : shell_presets_prerequirements_check.h
 *
 *
 */

#ifndef _shell_presets_prerequirements_check_H
#define _shell_presets_prerequirements_check_H



/***************************/
/******** test defines *****/



/***************************/




/***************************/
/*** test type existence (suffix should be unique)***/



/***************************/


/**********************************/
/*** test variable/function declaration existence (suffix should be unique)***/


#ifndef os_safe_malloc
enum{
	os_safe_malloc___not_declared_in_os_wrapper_h__as_required_by_shell_presets = sizeof(os_safe_malloc)
};
#endif

#ifndef os_safe_free
enum{
	os_safe_free___not_declared_in_os_wrapper_h__as_required_by_shell_presets = sizeof(os_safe_free)
};
#endif


/***************************/




#endif /* */
