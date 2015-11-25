
#ifndef _internal_flash_API_H_
#define _internal_flash_API_H_

/**********      defines      ************/

/**********  define API  types ************/


/**********  define API  functions  ************/



uint32_t internal_flash_stm32f10x_api_SetCorrectLatency(uint32_t aClock_hz); // should be called before setting new clock
uint32_t internal_flash_stm32f10x_api_StartProgramming(uint32_t start_addr , uint32_t total_size_to_programm);
uint32_t internal_flash_stm32f10x_api_ContinueProgramming(uint8_t *buff,uint32_t size);


#endif
