
INCLUDE_THIS_COMPONENT := $(CONFIG_STM8)


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = stm8_interrupt_vector.c
SRC += low_level_calls.c
VPATH = src


include $(COMMON_CC)
