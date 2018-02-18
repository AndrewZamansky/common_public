ifeq ($(sort $(CONFIG_POLEG)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_TIMER)
endif

#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = TimerPoleg.c


VPATH = src 

include $(COMMON_CC)
