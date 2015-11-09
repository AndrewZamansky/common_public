/*
 * file : dev_managment_prerequirements_check.h
 *
 *
 *
 *
 *
 *
 */

#ifndef _dev_managment_prerequirements_check_H
#define _dev_managment_prerequirements_check_H



/***************************/
/******** test defines *****/



#ifndef DEV_CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES
	#define DEV_CONFIG_MAX_NUM_OF_DEVICES 	16
#endif

#ifndef DEV_CONFIG_NUM_OF_STATIC_DEVICES
	#define DEV_CONFIG_NUM_OF_STATIC_DEVICES 	0
#endif

#ifndef DEV_CONFIG_MAX_DEV_NAME_LEN
	#define DEV_CONFIG_MAX_DEV_NAME_LEN			8
#endif

#ifndef NULL
	#error  "NULL is not defined in dev_managment_config.h "
#endif

/***************************/




/***************************/
/*** test type existence ***/

typedef uint8_t _if_fail_here_then___uint8_t___is_not_defined_in_dev_managment_config_h;
typedef uint16_t _if_fail_here_then___uint16_t___is_not_defined_in_dev_managment_config_h;
typedef uint32_t _if_fail_here_then___uint32_t___is_not_defined_in_dev_managment_config_h;

/***************************/


/**********************************/
/*** test declaration existence ***/


enum
{
	strcmp_not_declared_in_dev_managment_config_h=sizeof((int)strcmp)
};

#if DEV_CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES > 0
	enum
	{
		snprintf___not_declared_in_dev_managment_config_h=sizeof((int)snprintf),
		strnlen___not_declared_in_dev_managment_config_h=sizeof((int)strnlen),
		strcpy___not_declared_in_dev_managment_config_h=sizeof((int)strcpy)
	};
#endif


/***************************/


#endif /* */
