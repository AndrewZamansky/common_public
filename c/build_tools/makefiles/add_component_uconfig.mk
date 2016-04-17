
HASH=\#
COMPONENT_PATH := $(call caclulate_dir,$(COMPONENT_CONFIG_FILE))
COMPONENT_NAME := $(notdir $(COMPONENT_PATH))
include $(COMPONENT_CONFIG_FILE)
ifeq ($(filter y,$(sort $(INCLUDE_THIS_COMPONENT))),y) 
  GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(COMPONENT_PATH) 
  SUBDIRS := $(SUBDIRS) $(COMPONENT_PATH) 
  ifdef DO_GENERATE_FILES
    ifeq ($(filter y,$(DYNAMIC_COMPONENT)),y)
      ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        FILE_CONTENT_H := $(FILE_CONTENT_H) echo \#include "$(COMPONENT_NAME)_api.h">>$(AUTO_GENERATED_FILES_DIR)/included_modules.h &
        FILE_CONTENT_C := $(FILE_CONTENT_C) echo      {"$(COMPONENT_NAME)" , $(COMPONENT_NAME)_api_init_dev_descriptor},>>$(AUTO_GENERATED_FILES_DIR)/included_modules.c &
      else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 
        FILE_CONTENT_H := $(FILE_CONTENT_H) echo '$(HASH)include "$(COMPONENT_NAME)_api.h"'>>$(AUTO_GENERATED_FILES_DIR)/included_modules.h ;
        FILE_CONTENT_C := $(FILE_CONTENT_C) echo '     {"$(COMPONENT_NAME)" , $(COMPONENT_NAME)_api_init_dev_descriptor},'>>$(AUTO_GENERATED_FILES_DIR)/included_modules.c &
      endif
    endif #if DYNAMIC_COMPONENT==YES
  endif#DO_GENERATE_FILES
endif 

ifeq ($(filter y,$(INCLUDE_THIS_FOR_H_FILES_PATH)),y) 
    GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(COMPONENT_PATH) 
endif
#$(info ----$(COMPONENT_NAME))
#$(info --INCLUDE_THIS_COMPONENT = $(INCLUDE_THIS_COMPONENT))
#$(info --INCLUDE_THIS_FOR_H_FILES_PATH = $(INCLUDE_THIS_FOR_H_FILES_PATH))
#$(info ++$(GLOBAL_INCLUDE_DIR))
#$(info ++$(SUBDIRS))
# 
#
INCLUDE_THIS_COMPONENT := NO 
DYNAMIC_COMPONENT := NO
INCLUDE_THIS_FOR_H_FILES_PATH := NO
