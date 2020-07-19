
ifeq ($(sort $(CONFIG_INTERNAL_FILE_DESCRIPTORS_MANAGER)),y)
    INCLUDE_THIS_COMPONENT :=y
endif

ifeq ($(sort $(CONFIG_INTERNAL_FILE_DESCRIPTORS_MANAGER)),y)
    ifeq ($(sort $(CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION)),y)
        CURR_COMPONENT_DIR :=\
            $(patsubst %/,%,$(dir $(realpath $(lastword $(MAKEFILE_LIST)))))
        DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH, $(CURR_COMPONENT_DIR)/include)
    endif
endif

#INCLUDE_DIR =  

# following macro was added from some gcc version (at least from 9.2)
# defining it will open typdefs like ulong
DUMMY := $(call ADD_TO_GLOBAL_DEFINES , _GNU_SOURCE )

#CFLAGS = 

#ASMFLAGS =  


ifeq ($(sort $(CONFIG_INTERNAL_FILE_DESCRIPTORS_MANAGER)),y)
    SRC += file_descriptor_manager.c
else
    SRC += dummy_file_descriptor_manager.c
endif
VPATH = src



IS_LIBRARY :=$(CONFIG_OUTPUT_TYPE_STATIC_LIBRARY)

ifdef COMMON_CC#  COMMON_CC is defined during compilation stage
IS_LIBRARY :=$(filter y,$(IS_LIBRARY))
ifeq ($(IS_LIBRARY),y)
all:
	@echo no need to compile file descriptors related code in library
else
    include $(COMMON_CC)
endif
endif #for ifdef COMMON_CC
