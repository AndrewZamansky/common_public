ifeq ($(sort $(CONFIG_NUC505)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_CLOCK_CONTROL)
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = clock_control_nuc505.c
VPATH = src

SRC += clk.c
SRC += spim.c
VPATH +=  | $(NUC505_SRC_DIR)

include $(COMMON_CC)
