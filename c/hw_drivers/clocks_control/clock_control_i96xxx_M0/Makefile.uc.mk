ifeq ($(sort $(CONFIG_I96XXX_M0)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_CLOCK_CONTROL)
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = clock_control_i96xxx_m0.c
VPATH = src

SRC += clk.c

VPATH += $(I96XXX_SRC_DIR)

include $(COMMON_CC)
