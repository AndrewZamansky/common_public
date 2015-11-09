ifeq ($(findstring stm32f10x,$(CONFIG_SOC_TYPE)),stm32f10x)
    ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_CLOCK_CONTROL)),YES) 	 
    	INCLUDE_THIS_COMPONENT := YES   # must be here !!
    endif  
endif

INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = clocks_control_stm32f10x.c
VPATH = src

SRC += stm32f10x_rcc.c
VPATH +=  | $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver/src

include $(COMMON_CC)
