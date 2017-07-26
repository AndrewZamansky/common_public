
INCLUDE_THIS_COMPONENT := $(CONFIG_XTENSA_XCC)


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  


SRC = xtensa_vectors.S
SRC += xtensa_intr_asm.S
SRC += xtensa_intr.c

VPATH = src_xcc

ifdef CONFIG_CPU_SPECIFIC_FILES_ARE_SPEAD_CRITICAL 
    #SPEED_CRITICAL_FILES := $(SPEED_CRITICAL_FILES) $(SRC)
endif

include $(COMMON_CC)
