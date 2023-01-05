ifeq ($(sort $(CONFIG_INCLUDE_INTERNAL_TIMER)),y)
    INCLUDE_THIS_COMPONENT :=$(CONFIG_XTENSA_XCC) $(CONFIG_XTENSA_CLANG)
endif

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = xtensa_internal_timers.c
SRC += xtensa_internal_timers.S
VPATH = src


include $(COMMON_CC)
