ifeq ($(findstring nuc505,$(CONFIG_SOC_TYPE)),nuc505)
    ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_GPIO)),YES) 	 
	    _INCLUDE_GPIO := YES
	    DYNAMIC_COMPONENT := YES
	    INCLUDE_THIS_COMPONENT := YES   # must be here !!
    endif  
endif


GLOBAL_INCLUDE_DIR += $(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003/Library/StdDriver/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = gpio_nuc505.c 
VPATH = src

SRC += gpio.c
VPATH += | $(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003/Library/StdDriver/src

include $(COMMON_CC)
