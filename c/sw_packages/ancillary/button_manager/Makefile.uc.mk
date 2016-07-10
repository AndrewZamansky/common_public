
ifeq ($(findstring YES,$(CONFIG_INCLUDE_BUTTON_MANAGER)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = button_manager.c

VPATH = src


include $(COMMON_CC)
