
include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk

#clear file
$(shell echo # > $(COMPONENTS_MK))
$(info creating include_components.mk)

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
    FILE_CONTENT :=$(patsubst %, echo include %>>$(COMPONENTS_MK) &,$(ALL_CONFIG_FILES))
    DUMMY:=$(shell $(FILE_CONTENT))
    
    $(shell echo INCLUDE_THIS_COMPONENT := NO>> $(COMPONENTS_MK))
    $(shell echo DYNAMIC_COMPONENT := NO>> $(COMPONENTS_MK))
    $(shell echo INCLUDE_THIS_FOR_H_FILES_PATH := NO>> $(COMPONENTS_MK))
    ADD_COMPONENT_UCONFIG :=echo include $$(MAKEFILE_DEFS_ROOT_DIR)/add_component_uconfig.mk>>$(COMPONENTS_MK) &
    FILE_CONTENT :=$(patsubst %, echo COMPONENT_CONFIG_FILE := %>>$(COMPONENTS_MK) & $(ADD_COMPONENT_UCONFIG),$(ALL_CONFIG_FILES))
    DUMMY:=$(shell $(FILE_CONTENT))
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 
    $(error -----  NEEDS TO BE DONE !!!!)
endif




CREATE_CONFIG_ENTRY1 :=
CREATE_CONFIG_ENTRY2 :=
CREATE_CONFIG_ENTRY_PRELIMINARY1 :=

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
 $(shell echo  /* */> $(AUTO_GENERATED_FILES_DIR)/included_modules.h)
 FILE_CONTENT := echo \#include "dev_managment_api.h">$(AUTO_GENERATED_FILES_DIR)/included_modules.c &
 FILE_CONTENT := $(FILE_CONTENT) echo \#include "included_modules.h">>$(AUTO_GENERATED_FILES_DIR)/included_modules.c &
 FILE_CONTENT := $(FILE_CONTENT) echo const included_module_t included_modules[]={  >>$(AUTO_GENERATED_FILES_DIR)/included_modules.c
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX) 
 $(shell echo  "/* */"> $(AUTO_GENERATED_FILES_DIR)/included_modules.h)
    $(error -----  NEEDS TO BE DONE !!!!)
# $(shell echo "#include \"dev_managment_api.h\"" > $(AUTO_GENERATED_FILES_DIR)/included_modules.c)
# $(shell echo "#include \"included_modules.h\"" >> $(AUTO_GENERATED_FILES_DIR)/included_modules.c)
# $(shell echo "const included_module_t included_modules[]={  ">> $(AUTO_GENERATED_FILES_DIR)/included_modules.c)
endif
DUMMY:=$(shell $(FILE_CONTENT))



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
 $(shell echo "    {\"non-existent-dev\" , NULL} ">> $(AUTO_GENERATED_FILES_DIR)/included_modules.c)
 $(shell echo "};  ">> $(AUTO_GENERATED_FILES_DIR)/included_modules.c)
endif

all :
	$(info auto generated include_components.mk created)
