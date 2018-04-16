
ifeq ($(sort $(CONFIG_I94XXX)),y)
    INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_INTERNAL_I2S)
endif



#INCLUDE_DIR =  

#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  


SRC += I2S_onSPI_i94xxx.c


ifdef CONFIG_INTERNAL_I2S_SPEED_CRITICAL
    SPEED_CRITICAL_FILES += I2S_onSPI_i94xxx.c
endif


VPATH += src

#check if spi.c should not be compile in another Makefile.uc.mk
ifneq ($(sort $(CONFIG_INCLUDE_INTERNAL_SPI)),y)
    SRC += spi.c
    VPATH += | $(I94XXX_SRC_DIR)
endif

include $(COMMON_CC)
