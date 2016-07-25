
ifeq ($(findstring YES,$(CONFIG_INCLUDE_LM35)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = LM35.c 

VPATH = src

include $(COMMON_CC)

