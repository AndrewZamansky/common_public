
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_HEARTBEAT)


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = heartbeat.c


VPATH = src

ifdef CONFIG_HEARTBEAT_IS_SPEED_CRITICAL 
    SPEED_CRITICAL_FILES += heartbeat.c
endif

include $(COMMON_CC)
