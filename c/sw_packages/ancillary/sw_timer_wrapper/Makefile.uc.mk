
ifeq ($(findstring YES,$(CONFIG_INCLUDE_SW_TIMER_WRAPPER)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = sw_timer_wrapper.c

VPATH = src


include $(COMMON_CC)
