include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk


ifdef CONFIG_ARM
    ifdef CONFIG_GCC
        include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_cc.mk
    else ifdef CONFIG_GPP
        include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_cc.mk
    else ifdef CONFIG_ARMCC
        include $(MAKEFILE_DEFS_ROOT_DIR)/armcc/armcc_cc.mk
    endif
else ifdef CONFIG_AVR
    include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_cc.mk
else ifdef CONFIG_PIC32
    include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_cc.mk
else ifdef CONFIG_STM8
    include $(MAKEFILE_DEFS_ROOT_DIR)/cxstm8/cxstm8_cc.mk
else ifdef CONFIG_HOST
    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        ifdef CONFIG_MIN_GW_GCC
            include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_cc.mk
        else ifdef CONFIG_MICROSOFT_COMPILER
            include $(MAKEFILE_DEFS_ROOT_DIR)/microsoft_compiler/mcc_cc.mk
        endif
    else
        include $(MAKEFILE_DEFS_ROOT_DIR)/gcc/gcc_cc.mk
    endif
else
    $(error ---- unknown compiler ----)
endif

$(eval $(CALCULATE_ALL_INCLUDE_DIRS))
$(eval $(CALCULATE_ALL_DEFINES))
$(eval $(CALCULATE_ALL_ASM_DEFINES))
$(eval $(CALCULATE_CC_OUTPUT_FLAG_AND_FILE))
$(eval $(CALCULATE_ASM_OUTPUT_FLAG_AND_FILE))


CURRENT_COMPILATION_DIR_NAME := $(notdir $(abspath .))

CURRENT_COMPILE_DIRS := . $(subst |,$(SPACE),$(VPATH))
LOCAL_HEADER_FILES_DEPS := $(patsubst %,%/*.h,$(INCLUDE_DIR) $(CURRENT_COMPILE_DIRS))
HEADER_FILES_DEPS := $(wildcard $(GLOBAL_HEADER_FILES_DEPS) $(LOCAL_HEADER_FILES_DEPS))


ifdef CREATING_ARCHIVE
    CURR_DIR := $(patsubst %/,%,$(dir $(abspath $(firstword $(MAKEFILE_LIST)))))
    LOCAL_DIRS_FOR_EXPORTS := $(filter $(WORKSPACE_ROOT_DIR)%,$(CURRENT_COMPILE_DIRS) $(INCLUDE_DIR))
    LOCAL_DIRS_FOR_EXPORTS := $(patsubst $(CURR_DIR)/%,%,$(realpath $(LOCAL_DIRS_FOR_EXPORTS)))
    LOCAL_DIRS_FOR_EXPORTS :=  $(patsubst $(WORKSPACE_ROOT_DIR)/%,$(WORKSPACE_NAME)/%,$(LOCAL_DIRS_FOR_EXPORTS))#make folder relative to workspace
endif

CURR_OBJ_DIR := $(OBJ_DIR)/$(CURRENT_COMPILATION_DIR_NAME)
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
    #replace backslash for slash
    CURR_OBJ_DIR := $(subst /,\,$(CURR_OBJ_DIR))
endif

ifeq ($(wildcard $(CURR_OBJ_DIR)),) 		#if $(CURR_OBJ_DIR) dont exists then $(wildcard $(CURR_OBJ_DIR)) will produce empty string
   DUMMY:=$(shell $(MKDIR)  $(CURR_OBJ_DIR)) # create   $(CURR_OBJ_DIR)
endif

SRC_C    :=$(filter %.c,$(SRC))
SRC_CC   :=$(filter %.cc,$(SRC))
SRC_CPP   :=$(filter %.cpp,$(SRC))
SRC_ASM    :=$(filter %.s,$(SRC)) $(filter %.S,$(SRC))
SRC_ASM_S    :=$(filter %.S,$(SRC))
SRC_OBJ := $(patsubst %.c,$(CURR_OBJ_DIR)/%.o,$(SRC_C))
SRC_CC_OBJ := $(patsubst %.cc,$(CURR_OBJ_DIR)/%.oo,$(SRC_CC))
SRC_CPP_OBJ := $(patsubst %.cpp,$(CURR_OBJ_DIR)/%.oop,$(SRC_CPP))
ASM_OBJ := $(patsubst %.s,$(CURR_OBJ_DIR)/%.o.asm,$(SRC_ASM))
ASM_OBJ_O := $(patsubst %.S,$(CURR_OBJ_DIR)/%.O.asm,$(SRC_ASM_S))

all: $(SRC_OBJ) $(SRC_CC_OBJ) $(SRC_CPP_OBJ) $(ASM_OBJ) $(ASM_OBJ_O)

ifeq ($(findstring $(WORKSPACE_NAME),$(LOCAL_DIRS_FOR_EXPORTS)),$(WORKSPACE_NAME))
archive :
	$(ARCHIVER) $(ARCHIVE_OUTPUT) $(LOCAL_DIRS_FOR_EXPORTS)
else
archive :
	@echo "no files to add"
endif



$(CURR_OBJ_DIR)/%.o.asm: %.s
	$(info -Compiling $<)
	$(ASM) $(GLOBAL_ASMFLAGS) $(ASMFLAGS)  $(ALL_ASM_DEFINES) $(ASM_OUTPUT_FLAG_AND_FILE) $<

$(CURR_OBJ_DIR)/%.O.asm: %.S
	$(info -Compiling $<)
	$(ASM) $(GLOBAL_ASMFLAGS) $(ASMFLAGS)  $(ALL_ASM_DEFINES) $(ASM_OUTPUT_FLAG_AND_FILE) $<

$(CURR_OBJ_DIR)/%.o: %.c $(HEADER_FILES_DEPS) $(APP_ROOT_DIR)/.config
	$(info -Compiling $<)
	$(CC) $(GLOBAL_CFLAGS) $(CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $(CC_OUTPUT_FLAG_AND_FILE) $<
#	open line to create preproccesor file
#	$(CC) -E -P $(GLOBAL_CFLAGS) $(CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $< -o  $@.pre 

$(CURR_OBJ_DIR)/%.oo: %.cc $(HEADER_FILES_DEPS) $(APP_ROOT_DIR)/.config
	$(info -Compiling $<)
	$(CC) $(GLOBAL_CFLAGS) $(CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $(CC_OUTPUT_FLAG_AND_FILE) $<
#	open line to create preproccesor file
#	$(CC) -E -P $(GLOBAL_CFLAGS) $(CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $< -o  $@.pre 

$(CURR_OBJ_DIR)/%.oop: %.cpp $(HEADER_FILES_DEPS) $(APP_ROOT_DIR)/.config
	$(info -Compiling $<)
	$(CC) $(GLOBAL_CFLAGS) $(CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $(CC_OUTPUT_FLAG_AND_FILE) $<
#	open line to create preproccesor file
#	$(CC) -E -P $(GLOBAL_CFLAGS) $(CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $< -o  $@.pre 
