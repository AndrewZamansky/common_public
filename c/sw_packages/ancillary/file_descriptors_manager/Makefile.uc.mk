
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



include $(COMMON_CC)
