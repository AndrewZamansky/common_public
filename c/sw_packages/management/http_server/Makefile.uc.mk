
ifeq ($(findstring YES,$(CONFIG_INCLUDE_HTTP)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  

INCLUDE_DIR = $(EXTERNAL_SOURCE_ROOT_DIR)/Fatfs/src

#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = HTTP.c 

VPATH = src

include $(COMMON_CC)
