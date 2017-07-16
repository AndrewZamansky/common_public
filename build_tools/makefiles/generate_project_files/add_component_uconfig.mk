
HASH=\#
COMPONENT_PATH := $(call calc_parent_dir,$(COMPONENT_CONFIG_FILE))
COMPONENT_NAME := $(notdir $(COMPONENT_PATH))
include $(COMPONENT_CONFIG_FILE)
ifeq ($(filter y,$(sort $(INCLUDE_THIS_COMPONENT))),y)
  GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(COMPONENT_PATH)
  SUBDIRS := $(SUBDIRS) $(COMPONENT_PATH)
endif 

ifeq ($(filter y,$(INCLUDE_THIS_FOR_H_FILES_PATH)),y) 
    GLOBAL_INCLUDE_DIR := $(GLOBAL_INCLUDE_DIR) $(COMPONENT_PATH)
endif

INCLUDE_THIS_COMPONENT := NO
INCLUDE_THIS_FOR_H_FILES_PATH := NO