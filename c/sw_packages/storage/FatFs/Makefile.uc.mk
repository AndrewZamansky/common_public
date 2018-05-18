ifeq ($(call eval_config,$(CONFIG_INCLUDE_FAT_FS)),y)
    INCLUDE_THIS_COMPONENT := y

    FAT_FS_VERSION :=$(patsubst "%",%,$(CONFIG_FAT_FS_VERSION))
    FAT_FS_PATH :=$(EXTERNAL_SOURCE_ROOT_DIR)/Fatfs_R$(FAT_FS_VERSION)

    ifeq ("$(wildcard $(FAT_FS_PATH))","")
        $(info !--- FatFs path $(FAT_FS_PATH) doesn't exist )
        $(info !--- download FatFs BSP version $(CONFIG_FAT_FS_VERSION) and unpack it to $(FAT_FS_PATH)  )
        $(info !--- make sure src and doc  folders is located in $(FAT_FS_PATH)/  after unpacking   )
        $(error )
    endif

    DUMMY := $(call ADD_TO_GLOBAL_INCLUDE_PATH , $(FAT_FS_PATH)/src )
endif


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  

# for ffconf.h :
INCLUDE += src

SRC += ff.c
SRC += diskio_implemented.c
SRC += FatFs.c

VPATH =  src | $(FAT_FS_PATH)/src

include $(COMMON_CC)
