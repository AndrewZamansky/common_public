
ifdef CONFIG_XCC_TOOLS_VER_2017_6
    TOOLS_VER :=RG-2017.6
else ifdef CONFIG_XCC_TOOLS_VER_2017_7
    TOOLS_VER :=RG-2017.7
else ifdef CONFIG_XCC_TOOLS_VER_RI_2018_0
    TOOLS_VER :=RI-2018.0
else ifdef CONFIG_XCC_TOOLS_VER_RI_2021_6
    TOOLS_VER :=RI-2021.6
else ifdef CONFIG_XCC_TOOLS_VER_RI_2022_9
    TOOLS_VER :=RI-2022.9
endif

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TOOLS_VER:=$(TOOLS_VER)-win32
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TOOLS_VER:=$(TOOLS_VER)-linux
endif

XCC_ROOT_DIR :=

####### test for existence of xtensa xcc compiler and  #####
####### put its directory name in XCC_ROOT_DIR     #####
SEARCHED_TOOL :=xt-xcc
SEARCHED_DIR_VARIABLE :=XCC_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE :=REDEFINE_XTENSA_XCC_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=$(TOOLS_VER)\XtensaTools\bin\xt-xcc.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=$(TOOLS_VER)/XtensaTools/bin/xt-xcc
endif
include $(MAKEFILES_ROOT_DIR)/_include_functions/tool_existence_check.mk
####### end of tool existence test #####

XCC_ROOT_DIR :=$(XCC_ROOT_DIR)/$(TOOLS_VER)/XtensaTools

TEMP := $(GLOBAL_MANUALLY_DEFINED_TOOL_DIR_LIST)
TEMP := $(filter-out $(REDEFINE_XTENSA_XCC_DIR),$(TEMP))
NEW_LIST := $(GLOBAL_MANUALLY_DEFINED_TOOL_DIR_LIST)
NEW_LIST += $(REDEFINE_XTENSA_XCC_DIR)/$(TOOLS_VER)/XtensaTools
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

ifdef CONFIG_XCC_OPTIMISE_WITH_DEBUG
    CONFIG_OPTIMIZE_LEVEL := Og
else ifdef CONFIG_XCC_OPTIMISE_SIZE
    CONFIG_OPTIMIZE_LEVEL := Os
else ifdef CONFIG_XCC_OPTIMIZE_SPEED_SLIGHTLY
    CONFIG_OPTIMIZE_LEVEL := O1
else ifdef CONFIG_XCC_OPTIMIZE_SPEED
    CONFIG_OPTIMIZE_LEVEL := O2
else ifdef CONFIG_XCC_OPTIMIZE_ULTRA_SPEED
    CONFIG_OPTIMIZE_LEVEL := O3
else
    CONFIG_OPTIMIZE_LEVEL :=O0
endif

GLOBAL_CFLAGS += -$(CONFIG_OPTIMIZE_LEVEL)


GLOBAL_CFLAGS += -g

ifeq (y,$(CONFIG_XCC_OPT_KEEP_INTERMEDIATE))
    GLOBAL_CFLAGS += -save-temps
endif
ifeq (y,$(CONFIG_XCC_OPT_INTER_PROCEDURAL_ANALYSIS))
    GLOBAL_CFLAGS += -ipa
endif
ifeq (y,$(CONFIG_XCC_OPT_USE_DSP_COPROCESSOR))
    GLOBAL_CFLAGS += -mcoproc
endif
ifeq (y,$(CONFIG_XCC_OPT_DONT_GENERATE_MEMW_INSTRUCTIONS))
    GLOBAL_CFLAGS += -mno-serialize-volatile
endif
ifeq (y,$(CONFIG_XCC_OPT_PUT_LITERALS_IN_TEXT_SECTION))
    GLOBAL_CFLAGS += -mtext-section-literals
endif
ifeq (y,$(CONFIG_XCC_OPT_FEEDBACK_INFO_TO_REORDER_FUNCTIONS))
    GLOBAL_CFLAGS += -fb_reorder
endif
ifeq (y,$(CONFIG_XCC_OPT_OPTIMIZES_FOR_SPACE))
    GLOBAL_CFLAGS += Os
endif
ifeq (y,$(CONFIG_XCC_OPT_ENABLE_OPTIMIZATION_ALIAS_RESTRICT))
    GLOBAL_CFLAGS += -OPT:alias=restrict
endif
ifeq (y,$(CONFIG_XCC_OPT_ENABLE_AUTOMATIC_VECTORIZATION))
    GLOBAL_CFLAGS += -LNO:simd
	GLOBAL_CFLAGS += -LNO:simd_v
endif
ifeq (y,$(CONFIG_XCC_OPT_SPECULATIVELY_VECTORIZING_LOOPS_WITH_IFS))
    GLOBAL_CFLAGS += -LNO:simd_agg_if_conv
endif
ifeq (y,$(CONFIG_XCC_OPT_ASSUME_PARAMETERS_ARE_ALIGNED))
    GLOBAL_CFLAGS += -LNO:aligned_formal_pointers=on
endif
ifeq (y,$(CONFIG_XCC_OPT_OPTIMIZE_FOR_CONNECTION_BOX))
    GLOBAL_CFLAGS += -mcbox
endif
ifeq (y,$(CONFIG_XCC_OPT_PRODUCE_W2C_FILE))
    GLOBAL_CFLAGS += -clist
endif
ifeq (y,$(CONFIG_XCC_OPT_ENABLE_LONG_CALLS))
    GLOBAL_CFLAGS += -mlongcalls
endif
ifeq (y,$(CONFIG_XCC_OPT_CREATE_SEPARATE_FUNCTION_SECTIONS))
    GLOBAL_CFLAGS +=  -ffunction-sections
endif

ifdef CONFIG_XTENSA_HIFI3_BD5
	XCC_CORE :=hifi3_bd5
else ifdef CONFIG_XTENSA_FUSIONF1_FPGA_2
	XCC_CORE :=FusionF1_FPGA_2
else ifdef CONFIG_XTENSA_HIFI3_NTCA
	XCC_CORE :=HIFI3_NTCA
else ifdef CONFIG_XTENSA_FUSIONF1_P7_G60_NTCA
	XCC_CORE :=FusionF1_P7_NAU83G60
else ifdef XTENSA_FUSIONF1_XRC_ALL_CACHE
	XCC_CORE :=XRC_FusionF1_All_cache
else
    $(info err: unknown core)
    $(call exit,1)
endif

CORE_CONFIG_DIR :=$(REDEFINE_XTENSA_CONFIGS_DIR)/$(TOOLS_VER)/$(XCC_CORE)/config
CORE_CONFIG_DIR :=$(call fix_path_if_in_windows,$(CORE_CONFIG_DIR))

ifeq ("$(wildcard $(CORE_CONFIG_DIR))","")
    $(info err: $(CORE_CONFIG_DIR) does not exists)
    $(call exit,1)
endif

ifeq ("","$(filter $(CORE_CONFIG_DIR),$(EXTERNAL_SRC_DIRS))")
    EXTERNAL_SRC_DIRS := $(EXTERNAL_SRC_DIRS) $(CORE_CONFIG_DIR)
    STR :=!!!!!!!!! ADD FOLLOWING INSTRUCTION TO PERFORM AFTER EXTRACTION:
    $(if $(findstring export_project,$(MAKECMDGOALS)),$(info $STR),)
    STR :=!!!!!!!!! 1) CREATE FOLDER $(REDEFINE_XTENSA_XCC_DIR)/$(TOOLS_VER)
    $(if $(findstring export_project,$(MAKECMDGOALS)),$(info $STR),)
    STR :=!!!!!!!!!    AND MOVE XtensaTools IN IT 
    $(if $(findstring export_project,$(MAKECMDGOALS)),$(info $STR),)
    STR :=!!!!!!!!! 2) CREATE FOLDER $(REDEFINE_XTENSA_CONFIGS_DIR)/$(TOOLS_VER)/$(XCC_CORE)
    $(if $(findstring export_project,$(MAKECMDGOALS)),$(info $STR),)
    STR :=!!!!!!!!!    AND MOVE config IN IT 
    $(if $(findstring export_project,$(MAKECMDGOALS)),$(info $STR),)
endif

GLOBAL_CFLAGS += --xtensa-core=$(XCC_CORE)
GLOBAL_CFLAGS += --xtensa-system=$(CORE_CONFIG_DIR)

$(eval $(call ADD_TO_GLOBAL_DEFINES , PROC_$(XCC_CORE)))
$(eval $(call ADD_TO_GLOBAL_DEFINES , CONFIG_$(XCC_CORE)))

#stop GLOBAL_CFLAGS and GLOBAL_CPPFLAGS calculation each time it used :
GLOBAL_CPPFLAGS := $(GLOBAL_CFLAGS)
GLOBAL_CFLAGS := $(GLOBAL_CFLAGS)

### GLOBAL_ASMFLAGS calculation

GLOBAL_ASMFLAGS := $(GLOBAL_CFLAGS) -Wa,--gdwarf-2

#end of flags definitions

SUB_CONFIG_DEFINES := PROC_$(XCC_CORE) CONFIG_$(XCC_CORE)
xtensa_remove_config = $(filter-out $(SUB_CONFIG_DEFINES),$(GLOBAL_DEFINES))
define xtensa_use_custom_config =
    $(eval GLOBAL_DEFINES := $(call xtensa_remove_config) PROC_$(1) CONFIG_$(1))
endef
# unexport xtensa_use_custom_config,
# otherwise will be expanded every recurcive call to makefile
unexport xtensa_use_custom_config 


CC   :=	$(XCC_ROOT_DIR)/bin/xt-xcc -c
CCPP := $(XCC_ROOT_DIR)/bin/xt-xc++ -c
ASM  :=	$(XCC_ROOT_DIR)/bin/xt-xcc -c
LD   :=	$(XCC_ROOT_DIR)/bin/xt-xc++
AR   :=	$(XCC_ROOT_DIR)/bin/xt-ar
