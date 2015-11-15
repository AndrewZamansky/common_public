
ifeq ($(findstring YES,$(CONFIG_INCLUDE_COMPRESSOR)),YES) 	 
    _INCLUDE_COMMON_DSP :=YES	 
    _INCLUDE_DSP_MANAGMENT :=YES	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  

#INCLUDE_DIR 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  


SPEED_CRITICAL_FILES += compressor.c


SRC = compressor.c

VPATH = src


include $(COMMON_CC)
