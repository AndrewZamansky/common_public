ifeq ($(findstring stm8,$(CONFIG_SOC_TYPE)),stm8)
    ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_ADC)),YES) 	 
        _INCLUDE_ADC := YES
    	DYNAMIC_COMPONENT := YES
    	INCLUDE_THIS_COMPONENT := YES   # must be here !!
    endif  
endif


INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = adc_stm8.c
VPATH = src

SRC += stm8s_adc1.c 
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/ST/stm8s-periphs/src

include $(COMMON_CC)
