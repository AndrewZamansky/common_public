
#ifndef _FLASH_API_H_
#define _FLASH_API_H_

/**********      defines      ************/

/**********  define API  types ************/


/**********  define API  functions  ************/



uint32_t FLASH_API_SetCorrectLatency(uint32_t aClock_hz); // should be called before setting new clock
uint32_t FLASH_API_StartProgramming(uint32_t start_addr , uint32_t total_size_to_programm);
uint32_t FLASH_API_ContinueProgramming(uint8_t *buff,uint32_t size);

#else
#pragma message( "warning : this header file had already been included" )
#endif
