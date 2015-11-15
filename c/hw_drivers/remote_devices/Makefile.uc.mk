
ifeq ($(findstring YES,$(_INCLUDE_REMOTE_DEVICES)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = remote_devices.c 
VPATH = src


include $(COMMON_CC)
