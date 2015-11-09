ifeq ($(findstring stm8,$(CONFIG_SOC_TYPE)),stm8)
    ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_GPIO)),YES) 	 
	    _INCLUDE_GPIO := YES
	    DYNAMIC_COMPONENT := YES
	    INCLUDE_THIS_COMPONENT := YES   # must be here !!
    endif  
endif


INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/ST/stm8s-periphs/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = gpio_stm8.c 
VPATH = src

SRC += stm8s_gpio.c
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/ST/stm8s-periphs/src

include $(COMMON_CC)
