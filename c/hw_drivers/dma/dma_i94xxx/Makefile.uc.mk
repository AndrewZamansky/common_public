ifeq ($(sort $(CONFIG_I94XXX)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_DMA)
endif


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = dma_i94xxx.c

VPATH = src

SRC += pdma.c

VPATH += | $(I94XXX_SRC_DIR)

ifdef CONFIG_INTERNAL_DMA_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += dma_i94xxx.c pdma.c
endif

include $(COMMON_CC)
