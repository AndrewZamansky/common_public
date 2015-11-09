
ifeq ($(findstring YES,$(CONFIG_INCLUDE_NVIC)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 
#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = nvic.c 
VPATH = src


include $(COMMON_CC)
