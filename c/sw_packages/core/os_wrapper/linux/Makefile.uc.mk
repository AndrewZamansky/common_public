
ifeq ($(sort $(CONFIG_LINUX_OS_WRAPPER)),y)
	INCLUDE_THIS_COMPONENT := y

    DUMMY := $(call ADD_TO_GLOBAL_LIBS , libpthread )
    DUMMY := $(call ADD_TO_GLOBAL_LIBS , librt )



    #INCLUDE_DIR =

    #DEFINES =

    #CFLAGS = -fpermissive

    #ASMFLAGS +=



    SRC += os_wrapper_linux.c 
    VPATH = src

endif


include $(COMMON_CC)
