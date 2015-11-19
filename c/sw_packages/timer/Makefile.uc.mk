
ifeq ($(findstring YES,$(CONFIG_INCLUDE_TIMER)),YES) 	 
	#DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = timer.c


VPATH = src

SPEED_CRITICAL_FILES += 

include $(COMMON_CC)
