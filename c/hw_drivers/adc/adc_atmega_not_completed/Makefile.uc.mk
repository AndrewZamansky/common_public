ifeq ($(findstring nuc505,$(CONFIG_SOC_TYPE)),nuc505)
    ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_ADC)),YES) 	 
        _INCLUDE_ADC := YES
    	INCLUDE_THIS_COMPONENT := YES   # must be here !!
    endif  
endif


INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = adc_atmega.c
VPATH = src

SRC += stm32f10x_adc.c 
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver/src

include $(COMMON_CC)
