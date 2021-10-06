
# always include this module because if CONFIG_INTERNAL_FILE_DESCRIPTORS_MANAGER
# not defined then required file function will be implemented as dummy
INCLUDE_THIS_COMPONENT :=y

# !!IMPORTANT!! : should be used before any 'include' statement in makefile:
CURR_COMPONENT_DIR :=$(call get_curr_component_dir)

ifeq ($(sort $(CONFIG_INTERNAL_FILE_DESCRIPTORS_MANAGER)),y)
    ifeq ($(sort $(CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION)),y)
        DUMMY :=\
           $(call ADD_TO_GLOBAL_INCLUDE_PATH, $(CURR_COMPONENT_DIR)/include)
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
