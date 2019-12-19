
INCLUDE_THIS_COMPONENT := $(CONFIG_INCLUDE_AUTO_INIT)


#DEFINES = 

#CFLAGS = 

#ASMFLAGS =  

# AUTO_INIT_WITH_BOUNDS is prefered as itis more robust
AUTO_INIT_WITH_BOUNDS :=
AUTO_INIT_WITH_BOUNDS += $(CONFIG_MICROSOFT_COMPILER)
AUTO_INIT_WITH_BOUNDS += $(CONFIG_HEXAGON_COMPILER)
AUTO_INIT_WITH_BOUNDS += $(CONFIG_ANDROID_NDK)
AUTO_INIT_WITH_BOUNDS += $(CONFIG_XTENSA_GCC)
ifdef CONFIG_ARM
    AUTO_INIT_WITH_BOUNDS += $(CONFIG_GCC)
    AUTO_INIT_WITH_BOUNDS += $(CONFIG_ARMCC)
endif
AUTO_INIT_WITH_BOUNDS :=$(strip $(AUTO_INIT_WITH_BOUNDS))

AUTO_INIT_WITHOUT_BOUNDS :=
AUTO_INIT_WITHOUT_BOUNDS +=$(CONFIG_XTENSA_XCC)
AUTO_INIT_WITHOUT_BOUNDS :=$(strip $(AUTO_INIT_WITHOUT_BOUNDS))


AUTO_INIT_WITHOUT_SECTION :=

ifeq ($(sort $(CONFIG_OUTPUT_IS_LIBRARY_FOR_EXPORT)),y)
    AUTO_INIT_WITH_BOUNDS :=
    AUTO_INIT_WITHOUT_BOUNDS :=
    AUTO_INIT_WITHOUT_SECTION :=y
endif

ifneq ($(AUTO_INIT_WITH_BOUNDS),)
    SRC = auto_init_with_section_bounds.c 
else ifneq ($(AUTO_INIT_WITHOUT_BOUNDS),)
    SRC = auto_init_without_section_bounds.c 
else ifneq ($(AUTO_INIT_WITHOUT_SECTION),)
    SRC = auto_init_without_section.c 
else
    $(info !--- unkonown compliler for auto_init infrastructure)
    $(error)
endif

USE_GCC_AUTO_INIT_HELPER :=
USE_MSVC_AUTO_INIT_HELPER :=
USE_GCC_AUTO_INIT_HELPER += $(CONFIG_HEXAGON_COMPILER)
USE_GCC_AUTO_INIT_HELPER += $(CONFIG_ANDROID_NDK)
USE_GCC_AUTO_INIT_HELPER += $(CONFIG_XTENSA_GCC)
ifeq ($(CONFIG_HOST),y)
    DUMMY := $(call ADD_TO_GLOBAL_DEFINES , COMPILING_FOR_HOST)
    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        USE_MSVC_AUTO_INIT_HELPER += $(CONFIG_MICROSOFT_COMPILER)
    else
        USE_GCC_AUTO_INIT_HELPER += $(CONFIG_LINUX_HOST_GCC)
    endif
endif
USE_GCC_AUTO_INIT_HELPER :=$(strip $(USE_GCC_AUTO_INIT_HELPER))
USE_MSVC_AUTO_INIT_HELPER :=$(strip $(USE_MSVC_AUTO_INIT_HELPER))


ifeq ($(sort $(CONFIG_OUTPUT_IS_LIBRARY_FOR_EXPORT)),y)
    USE_MSVC_AUTO_INIT_HELPER :=
    USE_GCC_AUTO_INIT_HELPER :=
    CONFIG_ARMCC :=
endif

ifneq ($(USE_MSVC_AUTO_INIT_HELPER),)
    SRC += auto_init_msvc_helper.c
endif
ifneq ($(USE_GCC_AUTO_INIT_HELPER),)
    SRC += auto_init_gcc_helper.c
endif
ifeq ($(CONFIG_ARMCC),y)
    SRC += auto_init_armcc_helper.c
endif


ifdef CONFIG_INCLUDE_ONLY_AUTO_INIT_API
    DEFINES += AUTO_INIT_PROJ_NAME=$(CONFIG_PROJECT_NAME)
    #only compile following file:
    SRC := auto_init_api_check.c
endif

ifeq ($(MAKECMDGOALS),all)
    include $(MAKEFILES_ROOT_DIR)/_functions/usefull_functions.mk
    rwildcard=$(wildcard $1$2)\
         $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

    ALL_AUTO_INIT_EXT_FILES := \
             $(strip $(call rwildcard,$(OBJ_DIR)/,*.auto_init.ext))
    ALL_AUTO_INIT_FILES := $(strip $(call rwildcard,$(OBJ_DIR)/,*.auto_init))

    ALL_AUTO_INIT_EXT_FILE := $(AUTO_GENERATED_FILES_DIR)/all_auto_init_ext.c
    ALL_AUTO_INIT_EXT_FILE := \
                 $(call fix_path_if_in_windows,$(ALL_AUTO_INIT_EXT_FILE))
    DUMMY := $(shell $(RM) $(ALL_AUTO_INIT_EXT_FILE) 2>&1)

    ALL_AUTO_INIT_FILE := $(AUTO_GENERATED_FILES_DIR)/all_auto_init.c
    ALL_AUTO_INIT_FILE := $(call fix_path_if_in_windows,$(ALL_AUTO_INIT_FILE))
    DUMMY := $(shell $(RM) $(ALL_AUTO_INIT_FILE) 2>&1)

    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        cat = $(shell type $(call fix_path_if_in_windows,$(1)) >> $(2))
    else
        cat = $(shell cat $(call fix_path_if_in_windows,$(1)) >> $(2))
    endif

    $(eval $(foreach \
        d,$(ALL_AUTO_INIT_EXT_FILES),$(call cat,$d,$(ALL_AUTO_INIT_EXT_FILE))))
    $(eval $(foreach \
        d,$(ALL_AUTO_INIT_FILES),$(call cat,$d,$(ALL_AUTO_INIT_FILE))))

    CURRENT_COMPILATION_DIR_NAME := $(notdir $(abspath .))
    AUTO_INIT_OBJ_FILE_DIR :=$(OBJ_DIR)/$(CURRENT_COMPILATION_DIR_NAME)
    AUTO_INIT_OBJ_FILE :=$(AUTO_INIT_OBJ_FILE_DIR)/auto_init_without_section.o
    AUTO_INIT_OBJ_FILE :=$(call fix_path_if_in_windows,$(AUTO_INIT_OBJ_FILE))
    SHELL_OUT :=$(shell $(RM) $(AUTO_INIT_OBJ_FILE) 2>&1)

endif

VPATH = src

include $(COMMON_CC)
