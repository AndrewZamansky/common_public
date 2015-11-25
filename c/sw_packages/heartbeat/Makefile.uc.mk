
ifeq ($(findstring YES,$(CONFIG_INCLUDE_HEARTBEAT)),YES) 	 
	#DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = heartbeat.c


VPATH = src

ifeq ($(findstring AS_SPEED_CRITICAL,$(CONFIG_INCLUDE_HEARTBEAT)),AS_SPEED_CRITICAL) 
    SPEED_CRITICAL_FILES += heartbeat.c
endif

include $(COMMON_CC)
