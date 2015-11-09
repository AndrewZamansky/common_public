ifeq ($(findstring poleg,$(CONFIG_SOC_TYPE)),poleg)
    ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_TIMER)),YES) 	 
	    INCLUDE_THIS_COMPONENT := YES   # must be here !!
    endif  
endif


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = TimerPoleg.c


VPATH = src 

include $(COMMON_CC)
