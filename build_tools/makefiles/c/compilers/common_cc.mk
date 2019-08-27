include $(MAKEFILES_ROOT_DIR)/common.mk


ifdef CONFIG_ARM
    ifdef CONFIG_GCC
        include $(COMPILERS_DIR)/gcc/gcc_cc.mk
    else ifdef CONFIG_GPP
        include $(COMPILERS_DIR)/gcc/gcc_cc.mk
    else ifdef CONFIG_ARMCC
        include $(COMPILERS_DIR)/armcc/armcc_cc.mk
    endif
else ifdef CONFIG_AVR
    include $(COMPILERS_DIR)/gcc/gcc_cc.mk
else ifdef CONFIG_XTENSA_GCC
    include $(COMPILERS_DIR)/gcc/gcc_cc.mk
else ifdef CONFIG_XTENSA_XCC
    include $(COMPILERS_DIR)/xcc/xcc_cc.mk
else ifdef CONFIG_PIC32
    include $(COMPILERS_DIR)/gcc/gcc_cc.mk
else ifdef CONFIG_STM8
    include $(COMPILERS_DIR)/cxstm8/cxstm8_cc.mk
else ifdef CONFIG_HEXAGON
    include $(COMPILERS_DIR)/clang/clang_hexagon_cc.mk
else ifdef CONFIG_ANDROID_NDK
    include $(COMPILERS_DIR)/gcc/gcc_cc.mk
else ifdef CONFIG_HOST
    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        ifdef CONFIG_MIN_GW_GCC
            include $(COMPILERS_DIR)/gcc/gcc_cc.mk
        else ifdef CONFIG_MICROSOFT_COMPILER
            include $(COMPILERS_DIR)/microsoft_compiler/msvc_cc.mk
        endif
    else
        include $(COMPILERS_DIR)/gcc/gcc_cc.mk
    endif
else
    $(info err: unknown compiler)
    $(call exit,1)
endif

$(eval $(CALCULATE_ALL_INCLUDE_DIRS))
$(eval $(CALCULATE_ALL_ASM_INCLUDE_DIRS))
$(eval $(CALCULATE_ALL_DEFINES))
$(eval $(CALCULATE_ALL_ASM_DEFINES))
$(eval $(CALCULATE_CC_OUTPUT_FLAG_AND_FILE))
$(eval $(CALCULATE_ASM_OUTPUT_FLAG_AND_FILE))


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
SRC_CC_OBJ := $(patsubst %.cc,$(CURR_OBJ_DIR)/%.oo,$(SRC_CC))
SRC_CPP_OBJ := $(patsubst %.cpp,$(CURR_OBJ_DIR)/%.oop,$(SRC_CPP))
ASM_OBJ := $(patsubst %.s,$(CURR_OBJ_DIR)/%.o.asm,$(SRC_ASM))
ASM_OBJ_O := $(patsubst %.S,$(CURR_OBJ_DIR)/%.O.asm,$(SRC_ASM_S))


ALL_DEPS :=$(HEADER_FILES_DEPS) $(APP_ROOT_DIR)/.config


all: $(SRC_OBJ) $(SRC_CC_OBJ) $(SRC_CPP_OBJ) $(ASM_OBJ) $(ASM_OBJ_O)


ASM_COMPILATION_CMD := $(ASM) $(GLOBAL_ASMFLAGS) $(ASMFLAGS) $(ALL_ASM_DEFINES)
ASM_COMPILATION_REDUCED_CMD :=$(call \
     reduce_cmd_len, $(ASM_COMPILATION_CMD) $(ALL_ASM_INCLUDE_DIRS))
LONG_ASM_CMD :=$(call check_win_cmd_len, $(ASM_COMPILATION_REDUCED_CMD))

ASM_ARGS_FILE :=$(strip $(if $(LONG_ASM_CMD),$(CURR_OBJ_DIR)/asm.args,))
ifeq ($(LONG_ASM_CMD),TOO_LONG)
    ASM_COMPILATION_CMD += $(CC_USE_ARGS_FROM_FILE_FLAG)$(ASM_ARGS_FILE)
else
    ASM_COMPILATION_CMD :=$(ASM_COMPILATION_REDUCED_CMD)
endif


$(CURR_OBJ_DIR)/%.o.asm: %.s $(ALL_DEPS) $(ASM_ARGS_FILE)
	$(info .    Compiling $<)
	$(call mkdir_if_not_exists, $(dir $@))
	$(ASM_COMPILATION_CMD) $(ASM_OUTPUT_FLAG_AND_FILE) $<

$(CURR_OBJ_DIR)/%.O.asm: %.S $(ALL_DEPS) $(ASM_ARGS_FILE)
	$(info .    Compiling $<)
	$(call mkdir_if_not_exists, $(dir $@))
	$(ASM_COMPILATION_CMD) $(ASM_OUTPUT_FLAG_AND_FILE) $<

$(ASM_ARGS_FILE): $(ALL_DEPS)
	$(eval D:=$(call fwrite,$(ASM_ARGS_FILE),$(ALL_ASM_INCLUDE_DIRS),TRUNCATE))



ALL_CFLAGS := $(GLOBAL_CFLAGS) $(CFLAGS)
C_COMPILATION_CMD := $(CC) $(ALL_CFLAGS) $(ALL_DEFINES)
C_COMPILATION_REDUCED_CMD :=$(call \
               reduce_cmd_len, $(C_COMPILATION_CMD) $(ALL_INCLUDE_DIRS))
LONG_C_CMD :=$(call check_win_cmd_len, $(C_COMPILATION_REDUCED_CMD))

C_ARGS_FILE :=$(strip $(if $(LONG_C_CMD),$(CURR_OBJ_DIR)/c.args,))
ifeq ($(LONG_C_CMD),TOO_LONG)
    C_COMPILATION_CMD += $(CC_USE_ARGS_FROM_FILE_FLAG)$(C_ARGS_FILE)
else
    C_COMPILATION_CMD :=$(C_COMPILATION_REDUCED_CMD)
endif


$(CURR_OBJ_DIR)/%.o: %.c $(ALL_DEPS) $(C_ARGS_FILE)
	$(info .    Compiling $<)
	$(call mkdir_if_not_exists, $(dir $@))
	$(eval SRC_FILE := $(realpath $<))
	$(info .    Compiling $(SRC_FILE))
	$(C_COMPILATION_CMD) $(CC_OUTPUT_FLAG_AND_FILE) $<

$(C_ARGS_FILE): $(ALL_DEPS)
	$(eval DUMMY := $(call fwrite,$(C_ARGS_FILE),$(ALL_INCLUDE_DIRS),TRUNCATE))




ALL_CFLAGS := $(GLOBAL_CFLAGS) $(CFLAGS)
CPP_COMPILATION_CMD := $(CCPP) $(ALL_CFLAGS) $(ALL_DEFINES)
CPP_COMPILATION_REDUCED_CMD :=$(call \
          reduce_cmd_len, $(CPP_COMPILATION_CMD) $(ALL_INCLUDE_DIRS))
LONG_CPP_CMD :=$(call check_win_cmd_len, $(CPP_COMPILATION_REDUCED_CMD))

CPP_ARGS_FILE :=$(strip $(if $(LONG_CPP_CMD),$(CURR_OBJ_DIR)/cpp.args,))
ifeq ($(LONG_CPP_CMD),TOO_LONG)
    CPP_COMPILATION_CMD += $(CC_USE_ARGS_FROM_FILE_FLAG)$(CPP_ARGS_FILE)
else
    CPP_COMPILATION_CMD :=$(CPP_COMPILATION_REDUCED_CMD)
endif


#	open line to create preproccesor file
#	$(CC) -E -P $(ALL_CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $< -o  $@.pre

$(CURR_OBJ_DIR)/%.oo: %.cc $(ALL_DEPS) $(CPP_ARGS_FILE)
	$(info .    Compiling $<)
	$(call mkdir_if_not_exists, $(dir $@))
	$(CPP_COMPILATION_CMD) $(CC_OUTPUT_FLAG_AND_FILE) $<
#	open line to create preproccesor file
#	$(CC) -E -P $(ALL_CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $< -o  $@.pre

$(CURR_OBJ_DIR)/%.oop: %.cpp $(ALL_DEPS) $(CPP_ARGS_FILE)
	$(info .    Compiling $<)
	$(call mkdir_if_not_exists, $(dir $@))
	$(CPP_COMPILATION_CMD) $(CC_OUTPUT_FLAG_AND_FILE) $<
#	open line to create preproccesor file
#	$(CC) -E -P $(ALL_CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $< -o  $@.pre

$(CPP_ARGS_FILE): $(ALL_DEPS)
	$(eval DUMMY :=$(call fwrite,$(CPP_ARGS_FILE),$(ALL_INCLUDE_DIRS),TRUNCATE))

.SECONDARY: $(ASM_ARGS_FILE) $(C_ARGS_FILE) $(CPP_ARGS_FILE)
