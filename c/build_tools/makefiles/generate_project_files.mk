
include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))


ALL_CONFIG_FILES := $(call rwildcard,$(APP_ROOT_DIR)/,Makefile.uc.mk)
ALL_CONFIG_FILES := $(ALL_CONFIG_FILES) $(call rwildcard,$(SW_PACKAGES_ROOT_DIR)/,Makefile.uc.mk)
ALL_CONFIG_FILES := $(ALL_CONFIG_FILES) $(call rwildcard,$(DRIVERS_ROOT_DIR)/,Makefile.uc.mk)
$(info scan for uconfig.mk done )


#clear file
$(info creating include_components.mk)
DOLLAR=$
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
    $(shell echo # > $(COMPONENTS_MK))
    SHELL_CMD_DELIMITER = &
    ADD_COMPONENT_UCONFIG :=echo include $$(MAKEFILE_DEFS_ROOT_DIR)/add_component_uconfig.mk>>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER)
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 
    $(shell echo '#' > $(COMPONENTS_MK))
    SHELL_CMD_DELIMITER = ;
    ADD_COMPONENT_UCONFIG :=echo 'include $$(MAKEFILE_DEFS_ROOT_DIR)/add_component_uconfig.mk'>>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER)
endif

# adding "include {PATH}/Makefile.uc.mk" lines to include_components.mk 
FILE_CONTENT :=$(patsubst %, echo include %>>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER),$(ALL_CONFIG_FILES))
DUMMY:=$(shell $(FILE_CONTENT))



$(shell echo DYNAMIC_COMPONENT := NO>> $(COMPONENTS_MK))
$(shell echo INCLUDE_THIS_FOR_H_FILES_PATH := NO>> $(COMPONENTS_MK))


# adding following lines to include_components.mk 
# "include $(MAKEFILE_DEFS_ROOT_DIR)/add_component_uconfig.mk "
# "COMPONENT_CONFIG_FILE := {PATH}/Makefile.uc.mk "
FILE_CONTENT :=$(patsubst %, echo COMPONENT_CONFIG_FILE := %>>$(COMPONENTS_MK) $(SHELL_CMD_DELIMITER) $(ADD_COMPONENT_UCONFIG),$(ALL_CONFIG_FILES))
DUMMY:=$(shell $(FILE_CONTENT))




CREATE_CONFIG_ENTRY1 :=
CREATE_CONFIG_ENTRY2 :=
CREATE_CONFIG_ENTRY_PRELIMINARY1 :=

# initializing "included_modules.c"  file 
HASH=\#
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
    $(shell echo  /* */> $(AUTO_GENERATED_FILES_DIR)/included_modules.h)
    FILE_CONTENT := echo \#include "dev_managment_api.h">$(AUTO_GENERATED_FILES_DIR)/included_modules.c &
    FILE_CONTENT := $(FILE_CONTENT) echo \#include "included_modules.h">>$(AUTO_GENERATED_FILES_DIR)/included_modules.c &
    FILE_CONTENT := $(FILE_CONTENT) echo const included_module_t included_modules[]={  >>$(AUTO_GENERATED_FILES_DIR)/included_modules.c
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 
    $(shell echo  '/* */'> $(AUTO_GENERATED_FILES_DIR)/included_modules.h)
    FILE_CONTENT := echo '$(HASH)include "dev_managment_api.h"'>$(AUTO_GENERATED_FILES_DIR)/included_modules.c ;
    FILE_CONTENT := $(FILE_CONTENT) echo '$(HASH)include "included_modules.h"'>>$(AUTO_GENERATED_FILES_DIR)/included_modules.c ;
    FILE_CONTENT := $(FILE_CONTENT) echo 'const included_module_t included_modules[]={  '>>$(AUTO_GENERATED_FILES_DIR)/included_modules.c
endif
DUMMY:=$(shell $(FILE_CONTENT))


# filling "included_modules.c"  and "included_modules.h" files 
DO_GENERATE_FILES := 1
FILE_CONTENT_H:=
FILE_CONTENT_C:=
include $(COMPONENTS_MK)
DUMMY:=$(shell $(FILE_CONTENT_H))
DUMMY:=$(shell $(FILE_CONTENT_C))



# finish to build include components file :
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
 $(shell echo     {"non-existent-dev" , NULL} >> $(AUTO_GENERATED_FILES_DIR)/included_modules.c)
 $(shell echo };  >> $(AUTO_GENERATED_FILES_DIR)/included_modules.c)
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 
 $(shell echo '    {"non-existent-dev" , NULL} '>> $(AUTO_GENERATED_FILES_DIR)/included_modules.c)
 $(shell echo '};  '>> $(AUTO_GENERATED_FILES_DIR)/included_modules.c)
endif


all :
	$(info auto generated include_components.mk created)
