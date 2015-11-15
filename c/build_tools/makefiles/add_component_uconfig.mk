

COMPONENT_PATH := $(call caclulate_dir,$(COMPONENT_CONFIG_FILE))
COMPONENT_NAME := $(notdir $(COMPONENT_PATH))
include $(COMPONENT_CONFIG_FILE)
ifeq ($(findstring YES,$(INCLUDE_THIS_COMPONENT)),YES) 
  GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(COMPONENT_PATH) 
  SUBDIRS := $(SUBDIRS) $(COMPONENT_PATH) 
  ifdef DO_GENERATE_FILES
    ifeq ($(findstring YES,$(DYNAMIC_COMPONENT)),YES) 
      ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        FILE_CONTENT_H := $(FILE_CONTENT_H) echo \#include "$(COMPONENT_NAME)_api.h">>$(AUTO_GENERATED_FILES_DIR)/included_modules.h &
        FILE_CONTENT_C := $(FILE_CONTENT_C) echo      {"$(COMPONENT_NAME)" , $(COMPONENT_NAME)_api_init_dev_descriptor},>>$(AUTO_GENERATED_FILES_DIR)/included_modules.c &
      else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 
        $(error -----  NEEDS TO BE DONE !!!!)
      endif
    endif #if DYNAMIC_COMPONENT==YES
  endif#DO_GENERATE_FILES
else
  ifeq ($(findstring YES,$(INCLUDE_THIS_FOR_H_FILES_PATH)),YES) 
   GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(COMPONENT_PATH) 
  endif
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
