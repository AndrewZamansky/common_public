
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_ESP8266)
 


#INCLUDE_DIR +=

# following macro was added from some gcc version (at least from 9.2)
# defining it will open typdefs like ulong
DEFINES += _GNU_SOURCE

#CFLAGS +=

#ASMFLAGS +=

SRC += ESP8266.c
VPATH = src


include $(COMMON_CC)
