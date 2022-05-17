include $(MAKEFILES_ROOT_DIR)/common.mk

USE_GCC_COMPILE := $(or $(CONFIG_GCC),$(CONFIG_GPP),$(CONFIG_AVR),\
      $(CONFIG_XTENSA_GCC),$(CONFIG_PIC32),$(CONFIG_ANDROID_NDK),\
      $(CONFIG_MIN_GW_GCC))
USE_GCC_COMPILE :=$(strip $(USE_GCC_COMPILE))# to remove all spaces
ifeq ($(USE_GCC_COMPILE),y)
    include $(COMPILERS_DIR)/gcc/gcc_cc.mk
else
    ifdef CONFIG_ARM
        ifdef CONFIG_ARMCC
            include $(COMPILERS_DIR)/armcc/armcc_cc.mk
        else
            $(info err: unknown compiler)
            $(call exit,1)
        endif
    else ifdef CONFIG_XTENSA_XCC
        include $(COMPILERS_DIR)/xcc/xcc_cc.mk
    else ifdef CONFIG_STM8
        include $(COMPILERS_DIR)/cxstm8/cxstm8_cc.mk
    else ifdef CONFIG_HEXAGON
        include $(COMPILERS_DIR)/clang/clang_hexagon_cc.mk
    else ifdef CONFIG_HOST
        ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
            ifdef CONFIG_MICROSOFT_COMPILER
                include $(COMPILERS_DIR)/microsoft_compiler/msvc_cc.mk
            endif
        else
            $(info err: unknown compiler)
            $(call exit,1)
        endif
    else
        $(info err: unknown compiler)
        $(call exit,1)
    endif
endif


ALL_INCLUDE_DIRS := $(call format_all_include_dir)
ALL_ASM_INCLUDE_DIRS := $(call format_all_asm_include_dir)
ALL_DEFINES := $(call format_all_defines)
ALL_ASM_DEFINES := $(call format_all_asm_defines)

CURRENT_COMPILATION_DIR_NAME := $(notdir $(abspath .))

CURRENT_COMPILE_DIRS := . $(subst |,$(SPACE),$(VPATH))
FOLDERS_FOR_LOCAL_HEADER_DEPS :=$(INCLUDE_DIR) $(CURRENT_COMPILE_DIRS);
LOCAL_HEADER_FILES_DEPS := $(patsubst %,%/*.h,$(FOLDERS_FOR_LOCAL_HEADER_DEPS))
LOCAL_HEADER_FILES_DEPS := $(wildcard $(LOCAL_HEADER_FILES_DEPS))

HEADER_FILES_DEPS := $(GLOBAL_HEADER_FILES_DEPS) $(LOCAL_HEADER_FILES_DEPS)


CURR_OBJ_DIR :=$(call fix_path_if_in_windows,\
                  $(OBJ_DIR)/$(CURRENT_COMPILATION_DIR_NAME))

$(call mkdir_if_not_exists, $(CURR_OBJ_DIR))

SRC_C    :=$(filter %.c,$(SRC))
SRC_CC   :=$(filter %.cc,$(SRC))
SRC_CPP   :=$(filter %.cpp,$(SRC))
SRC_ASM    :=$(filter %.s,$(SRC)) $(filter %.S,$(SRC))
SRC_ASM_S    :=$(filter %.S,$(SRC))
SRC_OBJ := $(patsubst %.c,$(CURR_OBJ_DIR)/%.o,$(SRC_C))
SRC_C_PREPROC := $(patsubst %.c,$(CURR_OBJ_DIR)/%.c.preproc,$(SRC_C))
SRC_CMDS := $(patsubst %.c,$(CURR_OBJ_DIR)/cmds/%.cmd,$(SRC_C))
SRC_CC_OBJ := $(patsubst %.cc,$(CURR_OBJ_DIR)/%.oo,$(SRC_CC))
SRC_CC_PREPROC := $(patsubst %.c,$(CURR_OBJ_DIR)/%.cc.preproc,$(SRC_CC))
SRC_CPP_OBJ := $(patsubst %.cpp,$(CURR_OBJ_DIR)/%.oop,$(SRC_CPP))
SRC_CPP_PREPROC := $(patsubst %.cpp,$(CURR_OBJ_DIR)/%.cpp.preproc,$(SRC_CPP))
ASM_OBJ := $(patsubst %.s,$(CURR_OBJ_DIR)/%.o.asm,$(SRC_ASM))
ASM_OBJ_O := $(patsubst %.S,$(CURR_OBJ_DIR)/%.O.asm,$(SRC_ASM_S))


ALL_DEPS :=$(HEADER_FILES_DEPS) $(APP_ROOT_DIR)/.config \
                $(ADDITIONAL_DEPS)


all: $(SRC_OBJ) $(SRC_CC_OBJ) $(SRC_CPP_OBJ) $(ASM_OBJ) $(ASM_OBJ_O)


########### start  ASM files #################
ALL_ASM_FLAGS := $(GLOBAL_ASMFLAGS) $(ASMFLAGS)
ALL_ASM_DEFS_AND_INCLUDES := $(ALL_ASM_DEFINES) $(ALL_ASM_INCLUDE_DIRS)
LONG_ASM_CMD :=$(call check_win_cmd_len, $(ALL_ASM_DEFS_AND_INCLUDES))
ASM_ARGS_FILE :=$(strip $(if $(LONG_ASM_CMD),$(CURR_OBJ_DIR)/asm.args,))
ifdef CONFIG_GCC
    ASM_ARGS_CONTENT :=$(ALL_ASM_DEFINES) $(subst \,/,$(ALL_ASM_INCLUDE_DIRS))
else
    ASM_ARGS_CONTENT :=$(ALL_ASM_DEFINES) $(ALL_ASM_INCLUDE_DIRS)
endif

$(CURR_OBJ_DIR)/%.o.asm: %.s $(ALL_DEPS) $(ASM_ARGS_FILE)
	$(info .    Compiling $<)
	$(call mkdir_if_not_exists, $(dir $@))
	$(call run_asm_compiler,$(ASM_ARGS_FILE),$<,$@)

$(CURR_OBJ_DIR)/%.O.asm: %.S $(ALL_DEPS) $(ASM_ARGS_FILE)
	$(info .    Compiling $<)
	$(call mkdir_if_not_exists, $(dir $@))
	$(call run_asm_compiler,$(ASM_ARGS_FILE),$<,$@)

$(ASM_ARGS_FILE): $(ALL_DEPS)
	$(eval D:=$(call fwrite,$(ASM_ARGS_FILE),$(ASM_ARGS_CONTENT),TRUNCATE))

###########  end of ASM files #############
#############################################


ALL_DEFS_AND_INCLUDES := $(ALL_DEFINES) $(ALL_INCLUDE_DIRS)
LONG_CMD :=$(call check_win_cmd_len, $(ALL_DEFS_AND_INCLUDES))
C_ARGS_FILE :=$(strip $(if $(LONG_CMD),$(CURR_OBJ_DIR)/c.args,))
ifdef CONFIG_GCC
    C_ARGS_CONTENT :=$(ALL_DEFINES) $(subst \,/,$(ALL_INCLUDE_DIRS))
else
    C_ARGS_CONTENT :=$(ALL_DEFINES) $(ALL_INCLUDE_DIRS)
endif


###########  start of C files #################
ALL_CFLAGS := $(GLOBAL_CFLAGS) $(CFLAGS)

  ####### extract info from preprocessed files
$(CURR_OBJ_DIR)/%.c.preproc: %.c $(ALL_DEPS) $(C_ARGS_FILE)
	$(info .    Preprocessing $<)
	$(call mkdir_if_not_exists, $(dir $@))
	$(call run_c_preprocessor,$(C_ARGS_FILE),$<,$@)

include $(MAKEFILES_ROOT_DIR)/c/compilers/preprocessor_analyzer.mk


C_DEPS := %.c $(CURR_OBJ_DIR)/%.c.analyzer $(ALL_DEPS) $(C_ARGS_FILE)
$(CURR_OBJ_DIR)/%.o: $(C_DEPS)
	$(info .    Compiling $<)
	$(eval SRC_FILE := $(realpath $<))
	$(info .    Compiling $(SRC_FILE))
	$(call run_c_compiler,$(C_ARGS_FILE),$<,$@)

###########  end of C files #################
#################################################


###########  start of CPP files #################

ALL_CPPFLAGS := $(GLOBAL_CPPFLAGS) $(CPPFLAGS)

  ####### extract info from preprocessed files
$(CURR_OBJ_DIR)/%.cc.preproc: %.cc $(ALL_DEPS) $(C_ARGS_FILE)
	$(info .    Preprocessing $<)
	$(call mkdir_if_not_exists, $(dir $@))
	$(call run_cpp_preprocessor,$(C_ARGS_FILE),$<,$@)


CC_DEPS := %.cc $(CURR_OBJ_DIR)/%.cc.analyzer $(ALL_DEPS) $(C_ARGS_FILE)
$(CURR_OBJ_DIR)/%.oo: $(CC_DEPS)
	$(info .    Compiling $<)
	$(call mkdir_if_not_exists, $(dir $@))
	$(call run_cpp_compiler,$(C_ARGS_FILE),$<,$@)


  ####### extract info from preprocessed files
$(CURR_OBJ_DIR)/%.cpp.preproc: %.cpp $(ALL_DEPS) $(C_ARGS_FILE)
	$(info .    Preprocessing $<)
	$(call mkdir_if_not_exists, $(dir $@))
	$(call run_cpp_preprocessor,$(C_ARGS_FILE),$<,$@)


CPP_DEPS := %.cpp $(CURR_OBJ_DIR)/%.cpp.analyzer $(ALL_DEPS) $(C_ARGS_FILE)
$(CURR_OBJ_DIR)/%.oop: $(CPP_DEPS)
	$(info .    Compiling $<)
	$(call mkdir_if_not_exists, $(dir $@))
	$(call run_cpp_compiler,$(C_ARGS_FILE),$<,$@)

###########  end of CPP files #############
#############################################
$(C_ARGS_FILE): $(ALL_DEPS)
	$(eval DUMMY :=$(call fwrite,$(C_ARGS_FILE),$(C_ARGS_CONTENT),TRUNCATE))

FILES_TO_KEEP := $(ASM_ARGS_FILE) $(C_ARGS_FILE) $(SRC_C_PREPROC)\
     $(SRC_CC_PREPROC) $(SRC_CPP_PREPROC)
.SECONDARY: $(FILES_TO_KEEP)
