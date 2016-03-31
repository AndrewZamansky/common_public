
ifeq ($(findstring stm8,$(CONFIG_CPU_TYPE)),stm8) 	 
	INCLUDE_THIS_COMPONENT := YES   # must be here !!
endif  


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = stm8_interrupt_vector.c
SRC += low_level_calls.c
VPATH = src


include $(COMMON_CC)
