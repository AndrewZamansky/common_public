ifeq ($(findstring YES,$(CONFIG_INCLUDE_GPIO_REMOTE)),YES) 	 
	_INCLUDE_REMOTE_DEVICES := YES
	DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = gpio_remote.c 
VPATH = src


include $(COMMON_CC)
