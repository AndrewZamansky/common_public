
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_ESP8266)
 


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = ESP8266.c
SRC += ESP8266_socket_api.c
VPATH = src


include $(COMMON_CC)
