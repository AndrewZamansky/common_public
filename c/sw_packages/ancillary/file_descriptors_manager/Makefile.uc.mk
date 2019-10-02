
INCLUDE_THIS_COMPONENT :=y


#INCLUDE_DIR =  

#DEFINES = 

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
