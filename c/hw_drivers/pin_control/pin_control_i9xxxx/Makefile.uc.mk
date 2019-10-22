ifeq ($(sort $(CONFIG_I94XXX) $(CONFIG_I96XXX_M0)),y)
    INCLUDE_THIS_COMPONENT := y
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = pin_control_i9xxxx.c
VPATH = src

include $(COMMON_CC)
