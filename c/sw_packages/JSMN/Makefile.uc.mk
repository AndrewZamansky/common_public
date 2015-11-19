
ifeq ($(findstring YES,$(CONFIG_INCLUDE_JSMN)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = jsmn.c

VPATH =  $(EXTERNAL_SOURCE_ROOT_DIR)/JSMN

include $(COMMON_CC)
