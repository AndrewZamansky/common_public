
#ifndef _Config_API_H_
#define _Config_API_H_

/**********  define API  types ************/

/**********  define API  functions  ************/

//typedef uint32_t (*ReadFileFromFS_t)(const char *fileName, uint8_t *pData,uint32_t MaxLengthToRead)  ;


uint8_t Config_API_Init(void);


#else
#pragma message( "warning : this header file had already been included" )
#endif
