
ifeq ($(findstring YES,$(CONFIG_INCLUDE_CORTEXM_SYSTICK)),YES) 	 
	_INCLUDE_TIMER := YES
	DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = cortexM_systick.c 
VPATH = src


include $(COMMON_CC)
