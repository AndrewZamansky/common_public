
ifeq ($(findstring YES,$(CONFIG_INCLUDE_MEMORY_POOL)),YES) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR = 

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = memory_pool.c

ifeq ($(findstring AS_SPEED_CRITICAL,$(CONFIG_INCLUDE_I2S_MIXER)),AS_SPEED_CRITICAL) 
    SPEED_CRITICAL_FILES += memory_pool.c
endif

VPATH = src

include $(COMMON_CC)
