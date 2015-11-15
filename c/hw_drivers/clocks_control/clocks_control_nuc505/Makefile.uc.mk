ifeq ($(findstring nuc505,$(CONFIG_SOC_TYPE)),nuc505)
    ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_CLOCK_CONTROL)),YES) 	 
    	INCLUDE_THIS_COMPONENT := YES   # must be here !!
    endif  
endif


#INCLUDE_DIR =$(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003/Library/StdDriver/inc

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = clocks_control_nuc505.c
VPATH = src

SRC += clk.c
SRC += spim.c
VPATH +=  | $(EXTERNAL_SOURCE_ROOT_DIR)/BSP_NUC505_v3.00.003/Library/StdDriver/src

include $(COMMON_CC)
