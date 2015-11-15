
ifeq ($(findstring YES,$(CONFIG_INCLUDE_HTTP)),YES) 	 
	DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = HTTP.c 

VPATH = src

include $(COMMON_CC)
