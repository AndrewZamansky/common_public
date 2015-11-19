
ifeq ($(findstring YES,$(CONFIG_INCLUDE_COMPRESSOR)),YES) 	 
    _INCLUDE_COMMON_DSP :=YES	 
    _INCLUDE_DSP_MANAGMENT :=YES	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  

#INCLUDE_DIR 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  




SRC = compressor.c

ifeq ($(findstring AS_SPEED_CRITICAL,$(CONFIG_INCLUDE_COMPRESSOR)),AS_SPEED_CRITICAL) 
    SPEED_CRITICAL_FILES += compressor.c
    _COMMON_DSP_IS_SPEED_CRITICAL := YES
endif

VPATH = src


include $(COMMON_CC)
