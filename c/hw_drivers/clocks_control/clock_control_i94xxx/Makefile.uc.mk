ifeq ($(sort $(CONFIG_I94XXX)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_CLOCK_CONTROL)
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = clock_control_i94xxx.c
VPATH = src

SRC += clk.c
SRC += fmc.c

VPATH +=  | $(I94XXX_SRC_DIR)

include $(COMMON_CC)
