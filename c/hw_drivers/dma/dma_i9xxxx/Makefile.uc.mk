ifeq ($(sort $(CONFIG_I94XXX) $(CONFIG_I96XXX_M0)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_DMA)
endif


#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  



SRC = dma_i9xxxx.c

VPATH = src

SRC += pdma.c

ifdef CONFIG_I94XXX
    VPATH += $(I94XXX_SRC_DIR)
else ifdef CONFIG_I96XXX_M0
    VPATH += $(I96XXX_SRC_DIR)
endif

ifdef CONFIG_INTERNAL_DMA_IS_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += dma_i9xxxx.c pdma.c
endif

include $(COMMON_CC)
