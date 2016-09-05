
ifeq ($(findstring YES,$(CONFIG_INCLUDE_ESP8266)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = ESP8266.c
VPATH = src


include $(COMMON_CC)
