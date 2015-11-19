
ifeq ($(findstring YES,$(CONFIG_INCLUDE_NVIC)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


ifeq ($(findstring stm32f10x,$(CONFIG_SOC_TYPE)),stm32f10x)
    INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/ST/STM32F10x_StdPeriph_Driver/inc
endif

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = nvic.c 
VPATH = src


include $(COMMON_CC)
