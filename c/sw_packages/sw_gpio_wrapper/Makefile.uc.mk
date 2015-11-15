
ifeq ($(findstring YES,$(CONFIG_INCLUDE_SW_GPIO_WRAPPER)),YES) 	 
	_INCLUDE_GPIO := YES
	DYNAMIC_COMPONENT := YES
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = sw_gpio_wrapper.c 
VPATH = src


include $(COMMON_CC)
