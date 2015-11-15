
ifeq ($(findstring YES,$(CONFIG_INCLUDE_HEARTBEAT)),YES) 	 
	DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = heartbeat.c


VPATH = src

SPEED_CRITICAL_FILES += heartbeat.c

include $(COMMON_CC)
