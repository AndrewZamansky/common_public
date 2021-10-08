/*
 *
 *   file  :  Config.cpp
 *
 */



/***************   includes    *******************/
#include "_project_typedefs.h"
#include "_project_defines.h"
#include "_project_func_declarations.h"
#include "errors_api.h"

#include "dev_management_api.h" // for device manager defines and typedefs
#define DEBUG
#include "PRINTF_api.h"

#include "config_api.h"
#include "config.h"
#include "jsmn.h"
#include "ff.h"
#include "os_wrapper.h"

/***************   defines    *******************/

#define CONFIG_TASK_PRIORITY				TASK_NORMAL_PRIORITY

/* The check task uses the sprintf function so requires a little more stack. */
#define CONFIG_TASK_STACK_SIZE			DEFINE_STACK_SIZE( 300 )


#define CONFIG_FILE_NAME   "BOARD.CFG"
#define MAX_JSMN_BUFFER_SIZE  1024
#define MAX_JSMN_TOKEN_ALLOWED  32

#define TOKEN_EQ(t, tok_start, tok_end, tok_type) \
	((t).start == tok_start \
	 && (t).end == tok_end  \
	 && (t).type == (tok_type))

#define TOKEN_STRING(js, t, s) \
	(strncmp((const char *)js+(t).start, s, (t).end - (t).start) == 0 \
	 && strlen(s) == (t).end - (t).start)

#define TOKEN_STRING_LENGTH_OBTAINED( js, t, s , len) \
	((0==s[len]) && memcmp(js+(t).start, s, len) == 0 )

/***************   typedefs    *******************/


/**********   external variables    **************/



/***********   local variables    **************/
static 	jsmntok_t *tokens;

#define MAX_BUFF_SIZE (16+15)// 16 for param name , 15 for value
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        Config_Task                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                         */
/*                                                                                                  */
/* Returns:                                                                                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                            						 */
/*---------------------------------------------------------------------------------------------------------*/
static void Config_Task( void *pvParameters )
{

	uint32_t i;
	uint32_t paramStatus;
	const struct dev_param_t *dev_Params;
	uint8_t dev_Num_Of_Params;
	uint8_t save_buff[MAX_BUFF_SIZE+1];
	FIL fp;
	UINT bytes_written;

	for( ;; )
	{

		struct dev_desc_t * currDev;
		vTaskDelay( 5000 );

		currDev=DevManagment_API_GetAllDevsArray();

		for(i=0;i< DEV_CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES; i++)
		{
			if( ( 0 != currDev->name[0] )&&
					(0==DEV_IOCTL, IOCTL_TEST_PARAMS_STATUS ,  &paramStatus )) )
			{
				if(PARAMS_STATUS_NO_CHANGED_AFTER_LAST_TEST == paramStatus)
				{
					snprintf((char*)save_buff,MAX_BUFF_SIZE,"%s.sav",currDev->name);
					if(FR_OK==f_open(&fp,(char*)save_buff, FA_WRITE | FA_CREATE_ALWAYS ))
					{
//						PRINTF_DBG("[\r\n");
						f_write(&fp,"[\r\n",sizeof("[\r\n")-1,&bytes_written);
						DEV_IOCTL, IOCTL_GET_PARAMS_ARRAY_FUNC , &dev_Params, &dev_Num_Of_Params );
						while(dev_Num_Of_Params)
						{
							if(FOR_SAVE == dev_Params->usedForSave )
							{

								f_write(&fp,"\"",1,&bytes_written);
//								PRINT_DATA_DBG("\"",1);
								f_write(&fp,dev_Params->paramStr,strlen((char*)dev_Params->paramStr),&bytes_written);
//								PRINT_DATA_DBG(dev_Params->paramStr,strlen((char*)dev_Params->paramStr));
								f_write(&fp,"\":\"",sizeof("\":\"")-1,&bytes_written);
//								PRINT_DATA_DBG("\":\"",sizeof("\":\"")-1);
								DEV_IOCTL(currDev, dev_Params->paramGetIoctl ,   save_buff);
								save_buff[MAX_BUFF_SIZE] = 0;
								f_write(&fp,save_buff,strlen((char*)save_buff),&bytes_written);
//								PRINT_DATA_DBG(save_buff,strlen((char*)save_buff));
								f_write(&fp,"\",\r\n",sizeof("\",\r\n")-1,&bytes_written);
//								PRINT_DATA_DBG("\",\r\n",sizeof("\",\r\n")-1);
							}
							dev_Num_Of_Params--;
							dev_Params++;
						}
//						PRINTF_DBG("]\r\n");
						f_write(&fp,"[\r\n",sizeof("[\r\n")-1,&bytes_written);
						f_close(&fp);
					}
				}
			}
			currDev++;
		}

		os_stack_test();

	}


}


/*
 * function : config_parse_params()
 *
 *
 */
uint32_t config_parse_params(uint8_t *config_buff ,  uint32_t maxTokenAvailable ,
		const uint8_t *apParamName, uint32_t *found_token_num )
{
	uint32_t currTokenNum;
	
	currTokenNum=1;
	jsmntok_t array_token;

	while(currTokenNum < (maxTokenAvailable-1) )
	{
		uint32_t currTokenLen;

		if(JSMN_STRING != tokens[currTokenNum].type)
		{
			return 1;
		}

		currTokenLen=tokens[currTokenNum].end - tokens[currTokenNum].start ;

		if(TOKEN_STRING_LENGTH_OBTAINED(config_buff, tokens[currTokenNum], apParamName ,currTokenLen))
		{
			currTokenNum++;
			*found_token_num = currTokenNum ;
			return 0;
		}

		currTokenNum++;
		//test child token
		if((JSMN_ARRAY == tokens[currTokenNum].type) || (JSMN_OBJECT == tokens[currTokenNum].type))
		{
			array_token=tokens[currTokenNum];
			currTokenNum++;
			while ((currTokenNum < maxTokenAvailable) && (tokens[currTokenNum].end < array_token.end ))
			{
				currTokenNum++;
			}
		}
		else if( JSMN_STRING == tokens[currTokenNum].type)
		{
			currTokenNum ++;
		}


	}

	return 1;
}

extern const struct included_module_t included_modules[];
/*
 * function : config_device()
 *
 *
 */
uint32_t config_device(uint8_t *config_buff , uint32_t buff_len)
{
	jsmn_parser parser;
	int maxTokenAvailable ;
	uint8_t *pParam;
	uint32_t found_token_num;
	struct dev_desc_t *   dev_descriptor;
	uint32_t retVal;
	jsmntok_t curr_token,array_token;
	const struct dev_param_t *dev_Params;
	uint8_t dev_Num_Of_Params;
	init_dev_descriptor_func_t afInitDev;
	const struct included_module_t *pIncluded_module;

	jsmn_init(&parser);
	maxTokenAvailable = jsmn_parse(&parser, (char*)config_buff ,buff_len ,
			tokens , MAX_JSMN_TOKEN_ALLOWED);

	if(0 >= maxTokenAvailable)
	{
		return 1;
	}

	retVal=config_parse_params(config_buff ,maxTokenAvailable, (uint8_t*)"type",&found_token_num);
	if(0 != retVal)
	{
		return retVal;
	}

	pParam=&config_buff[tokens[found_token_num].start];
	pParam[tokens[found_token_num].end - tokens[found_token_num].start] = 0;

	pIncluded_module = included_modules;
	afInitDev = pIncluded_module->init_dev_descriptor_func;
	while(NULL != afInitDev)
	{
		if(0 == strcmp((char*)pParam , pIncluded_module->component_name))
		{
			break;
		}
		pIncluded_module++;
		afInitDev = pIncluded_module->init_dev_descriptor_func;

	}

	if(NULL == afInitDev)
	{
		return 1 ;
	}

	retVal=config_parse_params(config_buff ,maxTokenAvailable, (uint8_t*)"name",&found_token_num);
	if(0==retVal)
	{
		pParam=&config_buff[tokens[found_token_num].start];
		pParam[tokens[found_token_num].end - tokens[found_token_num].start] = 0;
	}
	else
	{
		pParam=NULL;
	}

	PRINTF_DBG("config_dev %s\n",pParam);
	dev_descriptor=DEV_API_add_device(pParam,afInitDev);
	if(NULL == dev_descriptor)
	{
		return 1 ;
	}

	//afInitDev(dev_descriptor);
	DEV_IOCTL(dev_descriptor , IOCTL_GET_PARAMS_ARRAY_FUNC , &dev_Params, &dev_Num_Of_Params );

//		PRINTF_DBG("aDev_Num_Of_Params=%d  \n",aDev_Num_Of_Params);
	while(dev_Num_Of_Params)
	{
		retVal=config_parse_params(config_buff  ,  maxTokenAvailable ,
						(uint8_t*)dev_Params->paramStr , &found_token_num);
//			PRINTF_DBG("config_dev avail=%d , found=%d\n",maxTokenAvailable,found_token_num);
		if(0 == retVal)
		{
			curr_token=tokens[found_token_num];
			if( JSMN_STRING == curr_token.type)
			{
				pParam=&config_buff[curr_token.start];
				pParam[curr_token.end - curr_token.start] = 0;
				PRINTF_DBG("%s\n",pParam);
				DEV_IOCTL(dev_descriptor , dev_Params->paramSetIoctl , pParam);
			}
			else if (JSMN_ARRAY == curr_token.type)
			{
				array_token=curr_token;
				found_token_num++;
				while ((found_token_num < maxTokenAvailable) && (tokens[found_token_num].end<array_token.end ))
				{
					curr_token=tokens[found_token_num];
					if( JSMN_STRING == curr_token.type)
					{
						pParam=&config_buff[curr_token.start];
						pParam[curr_token.end - curr_token.start] = 0;
						PRINTF_DBG("arr %s\n",pParam);
						DEV_IOCTL(dev_descriptor , dev_Params->paramSetIoctl , pParam);
					}
					found_token_num++;
				}
			}
		}
		dev_Num_Of_Params--;
		dev_Params++;
	}

	DEV_IOCTL(dev_descriptor , IOCTL_DEVICE_START );
	return 0;
}

/*
 * function : config_saved_params()
 *
 *
 */
uint32_t config_saved_params(uint8_t *config_buff)
{
	uint32_t i;
	struct dev_desc_t * currDev;
	uint32_t readLen;
	FIL fp;
	uint32_t retVal=0;
	jsmn_parser parser;
	uint32_t maxTokenAvailable ;
	const struct dev_param_t *dev_Params;
	uint8_t dev_Num_Of_Params;
	uint32_t found_token_num;
	jsmntok_t curr_token;
	uint8_t *pParam;

	currDev=DevManagment_API_GetAllDevsArray();
	for(i=0;i< DEV_CONFIG_MAX_NUM_OF_DYNAMIC_DEVICES; i++)
	{
		snprintf((char*)config_buff,MAX_JSMN_BUFFER_SIZE,"%s.sav",currDev->name);
		if ( 0 != currDev->name[0] )
		{
			if(FR_OK==f_open(&fp,(char*)config_buff, FA_READ ))
			{

				f_read(&fp,config_buff, MAX_JSMN_BUFFER_SIZE , (UINT*)&readLen);
				jsmn_init(&parser);
				maxTokenAvailable = jsmn_parse(&parser, (char*)config_buff ,readLen ,
						tokens , MAX_JSMN_TOKEN_ALLOWED);
				DEV_IOCTL, IOCTL_GET_PARAMS_ARRAY_FUNC , &dev_Params, &dev_Num_Of_Params );
				while(dev_Num_Of_Params)
				{
					if(FOR_SAVE == dev_Params->usedForSave )
					{
						retVal=config_parse_params(config_buff  ,  maxTokenAvailable ,
										(uint8_t*)dev_Params->paramStr , &found_token_num);
						PRINTF_DBG("config_saved_params avail=%d , found=%d\n",maxTokenAvailable,found_token_num);
						if(0 == retVal)
						{
							curr_token=tokens[found_token_num];
							if( JSMN_STRING == curr_token.type)
							{
								pParam=&config_buff[curr_token.start];
								pParam[curr_token.end - curr_token.start] = 0;
								PRINTF_DBG("%s = %s\n",dev_Params->paramStr,pParam);
								DEV_IOCTL, dev_Params->paramSetIoctl , pParam);
							}
						}
					}
					dev_Num_Of_Params--;
					dev_Params++;
				}
				f_close(&fp);
			}
		}
		currDev++;
	}

	return 0;
}


/*
 * function : Config_API_Init()
 *
 *
 */
uint8_t Config_API_Init(void)
{
	uint8_t retVal=0;
	uint8_t *cfg_buff;
	int buff_len ;
	uint32_t readLen;
	FIL fp;
	uint32_t braces_stack;

	tokens=(jsmntok_t*)os_safe_malloc(sizeof(jsmntok_t)*MAX_JSMN_TOKEN_ALLOWED);
	errors_api_check_if_malloc_succeed(tokens);
	cfg_buff=(uint8_t*)os_safe_malloc(MAX_JSMN_BUFFER_SIZE);
	errors_api_check_if_malloc_succeed(cfg_buff);

	if(FR_OK!=f_open(&fp,CONFIG_FILE_NAME, FA_READ))
	{
		goto error;
	}

	readLen=1;
	while(readLen)
	{
		readLen=1;
		cfg_buff[0]=0;
		while(readLen && ('{'!=cfg_buff[0]) )
		{
			f_read(&fp,&cfg_buff[0], 1 , (UINT*)&readLen);
		}
		braces_stack=1;
		buff_len=1;
		while(readLen && braces_stack )
		{
			f_read(&fp,&cfg_buff[buff_len], 1 , (UINT*)&readLen);
			if('{' == cfg_buff[buff_len])
			{
				braces_stack++;
			}
			else if('}' == cfg_buff[buff_len])
			{
				braces_stack--;
			}
			buff_len++;
		}
		cfg_buff[buff_len]=0;
		buff_len++;
//		PRINTF_DBG("params = %s  \n",cfg_buff  );

		if(readLen)
		{
			retVal|=config_device(cfg_buff , buff_len  );
		}
	}

	f_close(&fp);

	retVal|=config_saved_params(cfg_buff);

	xTaskCreate( Config_Task, "Config_Task", CONFIG_TASK_STACK_SIZE,(void*) NULL,
			CONFIG_TASK_PRIORITY , NULL );

	goto exit;
error :
	retVal=1;
exit:
	os_safe_free(tokens);
	os_safe_free(cfg_buff);
	return retVal;
}

