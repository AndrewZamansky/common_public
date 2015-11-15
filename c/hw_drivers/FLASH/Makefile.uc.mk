
ifeq ($(findstring YES,$(CONFIG_INCLUDE_INTERNAL_FLASH)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = flash.c 

VPATH = src


include $(COMMON_CC)
