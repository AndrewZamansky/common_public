
ifeq ($(findstring YES,$(CONFIG_INCLUDE_GIC)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES += 

#CFLAGS += 

#ASMFLAGS +=  



SRC = gic.c 


VPATH = src:APP_SPECIFIC

include $(COMMON_CC)
