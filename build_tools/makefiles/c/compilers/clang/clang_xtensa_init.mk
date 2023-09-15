
ifdef CONFIG_CLANG_TOOLS_VER_2017_6
    TOOLS_VER :=RG-2017.6
else ifdef CONFIG_CLANG_TOOLS_VER_2017_7
    TOOLS_VER :=RG-2017.7
else ifdef CONFIG_CLANG_TOOLS_VER_RI_2018_0
    TOOLS_VER :=RI-2018.0
else ifdef CONFIG_CLANG_TOOLS_VER_RI_2021_6
    TOOLS_VER :=RI-2021.6
else ifdef CONFIG_CLANG_TOOLS_VER_RI_2022_9
    TOOLS_VER :=RI-2022.9
else ifdef CONFIG_CLANG_TOOLS_VER_RI_2022_10
    TOOLS_VER :=RI-2022.10
else
    $(info err: clang tool version not supported)
    $(call exit,1)
endif

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TOOLS_VER:=$(TOOLS_VER)-win32
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TOOLS_VER:=$(TOOLS_VER)-linux
endif

CLANG_ROOT_DIR :=

####### test for existence of xtensa clang compiler and  #####
####### put its directory name in CLANG_ROOT_DIR     #####
SEARCHED_TOOL :=xt-clang
SEARCHED_DIR_VARIABLE :=CLANG_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE :=REDEFINE_XTENSA_CLANG_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=$(TOOLS_VER)\XtensaTools\bin\xt-clang.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=$(TOOLS_VER)/XtensaTools/bin/xt-clang
endif
include $(MAKEFILES_ROOT_DIR)/_include_functions/tool_existence_check.mk
####### end of tool existence test #####

CLANG_ROOT_DIR :=$(CLANG_ROOT_DIR)/$(TOOLS_VER)/XtensaTools

TEMP := $(GLOBAL_MANUALLY_DEFINED_TOOL_DIR_LIST)
TEMP := $(filter-out $(REDEFINE_XTENSA_CLANG_DIR),$(TEMP))
NEW_LIST := $(GLOBAL_MANUALLY_DEFINED_TOOL_DIR_LIST)
NEW_LIST += $(REDEFINE_XTENSA_CLANG_DIR)/$(TOOLS_VER)/XtensaTools
GLOBAL_MANUALLY_DEFINED_TOOL_DIR_LIST := $(NEW_LIST)


ifeq ("$(strip $(REDEFINE_XTENSA_CONFIGS_DIR))","")
    TMP_VAR_NAME :=$(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk
    TMP_VAR_NAME :=$(TMP_VAR_NAME)/REDEFINE_XTENSA_CONFIGS_DIR
    $(info err: $(TMP_VAR_NAME) is empty or does not exists)
    $(info ---: define Xtensa config dir in $(TMP_VAR_NAME) that contains)
    $(info ---: $(TOOLS_VER) folder of xtensa core build configurations)
    $(call exit,1)
endif

ifeq ("$(wildcard $(REDEFINE_XTENSA_CONFIGS_DIR))","")
    TMP_VAR_NAME :=$(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk
    TMP_VAR_NAME :=$(TMP_VAR_NAME)/REDEFINE_XTENSA_CONFIGS_DIR
    $(info err: $(REDEFINE_XTENSA_CONFIGS_DIR) does not exists)
    $(info ---: define Xtensa config dir in $(TMP_VAR_NAME) that contains)
    $(info ---: $(TOOLS_VER) folder of xtensa core build configurations)
    $(call exit,1)
endif



### GLOBAL_CFLAGS calculation

ifdef CONFIG_CLANG_OPTIMIZE_NONE
    CONFIG_OPTIMIZE_LEVEL := O0
else ifdef CONFIG_CLANG_OPTIMIZE_SIZE
    CONFIG_OPTIMIZE_LEVEL := Os
else ifdef CONFIG_CLANG_OPTIMIZE_SPEED
    CONFIG_OPTIMIZE_LEVEL := O2
else ifdef CONFIG_CLANG_OPTIMIZE_ULTRA_SPEED
    CONFIG_OPTIMIZE_LEVEL := O3
endif

GLOBAL_CFLAGS += -$(CONFIG_OPTIMIZE_LEVEL)


GLOBAL_CFLAGS += -g
GLOBAL_CFLAGS += -fmessage-length=0 


ifdef HIFI3_NTCA
	CLANG_CORE :=HIFI3_NTCA
	XTENSA_CURR_ACTIVE_CONFIG :=$(CLANG_CORE)
else ifdef CONFIG_XTENSA_FUSIONF1_P7_G60_NTCA
	CLANG_CORE :=FusionF1_P7_NAU83G60
	XTENSA_CURR_ACTIVE_CONFIG :=$(CLANG_CORE)
else ifdef CONFIG_XTENSA_FUSIONF1_XRC_ALL_CACHE
	CLANG_CORE :=XRC_FusionF1_All_cache
	XTENSA_CURR_ACTIVE_CONFIG :=$(CLANG_CORE)
else ifdef CONFIG_XTENSA_APP_DEPENDENT_CONFIG
    ifndef CONFIG_XTENSA_CORE_CONFIG
        $(info err: project defined to use application dependent xtensa config)
        $(info err: so CONFIG_XTENSA_CORE_CONFIG needs do be defined)
        $(call exit,1)
    endif
    CLANG_CORE :=$(patsubst "%",%,$(CONFIG_XTENSA_CORE_CONFIG))
	XTENSA_CURR_ACTIVE_CONFIG :=$(CLANG_CORE)
	
    MEMMAP_CONFIG :=$(patsubst "%",%,$(CONFIG_XTENSA_MEMMAP_CONFIG))
    ifneq ($(MEMMAP_CONFIG),)
        XTENSA_CURR_ACTIVE_CONFIG :=$(MEMMAP_CONFIG)
    else
        $(info info: using $(XTENSA_CURR_ACTIVE_CONFIG) as active config,)
        $(info info: you can select other config by CONFIG_XTENSA_MEMMAP_CONFIG)
    endif
	
else
    $(info err: unknown core)
    $(call exit,1)
endif

CORE_CONFIG_DIR :=\
          $(REDEFINE_XTENSA_CONFIGS_DIR)/$(TOOLS_VER)/$(CLANG_CORE)/config
CORE_CONFIG_DIR :=$(call fix_path_if_in_windows,$(CORE_CONFIG_DIR))

ifeq ("$(wildcard $(CORE_CONFIG_DIR))","")
    $(info err: $(CORE_CONFIG_DIR) does not exists)
    $(call exit,1)
endif

ifeq ("","$(filter $(CORE_CONFIG_DIR),$(EXTERNAL_SRC_DIRS))")
    EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(CORE_CONFIG_DIR)
    STR :=!!!!!!!!! ADD FOLLOWING INSTRUCTION TO PERFORM AFTER EXTRACTION:
    $(if $(findstring export_project,$(MAKECMDGOALS)),$(info $STR),)
    STR :=!!!!!!!!! 1) CREATE FOLDER $(REDEFINE_XTENSA_CLANG_DIR)/$(TOOLS_VER)
    $(if $(findstring export_project,$(MAKECMDGOALS)),$(info $STR),)
    STR :=!!!!!!!!!    AND MOVE XtensaTools IN IT 
    $(if $(findstring export_project,$(MAKECMDGOALS)),$(info $STR),)
    STR :=!!!!!!!!! 2) CREATE FOLDER $(REDEFINE_XTENSA_CONFIGS_DIR)/$(TOOLS_VER)/$(CLANG_CORE)
    $(if $(findstring export_project,$(MAKECMDGOALS)),$(info $STR),)
    STR :=!!!!!!!!!    AND MOVE config IN IT 
    $(if $(findstring export_project,$(MAKECMDGOALS)),$(info $STR),)
endif

GLOBAL_CFLAGS += --xtensa-core=$(CLANG_CORE)
GLOBAL_CFLAGS += --xtensa-system=$(CORE_CONFIG_DIR)

$(eval $(call ADD_TO_GLOBAL_DEFINES, PROC_$(XTENSA_CURR_ACTIVE_CONFIG)))
$(eval $(call ADD_TO_GLOBAL_DEFINES, CONFIG_$(XTENSA_CURR_ACTIVE_CONFIG)))

GLOBAL_CFLAGS += --xtensa-params=

#stop GLOBAL_CFLAGS and GLOBAL_CPPFLAGS calculation each time it used :
GLOBAL_CPPFLAGS := $(GLOBAL_CFLAGS)
GLOBAL_CFLAGS := $(GLOBAL_CFLAGS)

### GLOBAL_ASMFLAGS calculation

GLOBAL_ASMFLAGS := $(GLOBAL_CFLAGS)

#end of flags definitions

SUB_CONFIG_DEFINES := PROC_$(CLANG_CORE) CONFIG_$(CLANG_CORE)
xtensa_remove_config = $(filter-out $(SUB_CONFIG_DEFINES),$(GLOBAL_DEFINES))
define xtensa_use_custom_config =
    $(eval GLOBAL_DEFINES := $(call xtensa_remove_config) PROC_$(1) CONFIG_$(1))
endef
# unexport xtensa_use_custom_config,
# otherwise will be expanded every recurcive call to makefile
unexport xtensa_use_custom_config 


CC   :=	$(CLANG_ROOT_DIR)/bin/xt-clang -c
CCPP := $(CLANG_ROOT_DIR)/bin/xt-clang++ -c
ASM  :=	$(CLANG_ROOT_DIR)/bin/xt-clang -c
LD   :=	$(CLANG_ROOT_DIR)/bin/xt-clang++
AR   :=	$(CLANG_ROOT_DIR)/bin/xt-ar
