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
LOCAL_HEADER_FILES_DEPS := $(patsubst %,%/*.h,$(INCLUDE_DIR) $(CURRENT_COMPILE_DIRS))
HEADER_FILES_DEPS := $(wildcard $(GLOBAL_HEADER_FILES_DEPS) $(LOCAL_HEADER_FILES_DEPS))



CURR_OBJ_DIR := $(OBJ_DIR)/$(CURRENT_COMPILATION_DIR_NAME)
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS) 	 
    #replace backslash for slash
    CURR_OBJ_DIR := $(subst /,\,$(CURR_OBJ_DIR))
endif

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

all: $(SRC_OBJ) $(SRC_CC_OBJ) $(SRC_CPP_OBJ) $(ASM_OBJ) $(ASM_OBJ_O)

ALL_ASM_INCLUDE_DIRS :=$(subst $(PUBLIC_SW_PACKAGES_DIR),%PUBLIC_SW_PACKAGES_DIR%,$(ALL_ASM_INCLUDE_DIRS))
ALL_ASM_INCLUDE_DIRS :=$(subst $(EXTERNAL_SOURCE_ROOT_DIR),%EXTERNAL_SOURCE_ROOT_DIR%,$(ALL_ASM_INCLUDE_DIRS))
ALL_ASM_INCLUDE_DIRS :=$(subst $(PUBLIC_DRIVERS_DIR),%PUBLIC_DRIVERS_DIR%,$(ALL_ASM_INCLUDE_DIRS))
ALL_ASM_INCLUDE_DIRS :=$(subst $(APP_ROOT_DIR),%APP_ROOT_DIR%,$(ALL_ASM_INCLUDE_DIRS))

$(CURR_OBJ_DIR)/%.o.asm: %.s
	$(info .    Compiling $<)
	$(SET_CC_ENV_VARS)& $(ASM) $(GLOBAL_ASMFLAGS) $(ASMFLAGS) $(ALL_ASM_INCLUDE_DIRS) $(ALL_ASM_DEFINES) $(ASM_OUTPUT_FLAG_AND_FILE) $<

$(CURR_OBJ_DIR)/%.O.asm: %.S
	$(info .    Compiling $<)
	$(SET_CC_ENV_VARS)& $(ASM) $(GLOBAL_ASMFLAGS) $(ASMFLAGS) $(ALL_ASM_INCLUDE_DIRS) $(ALL_ASM_DEFINES) $(ASM_OUTPUT_FLAG_AND_FILE) $<


ALL_INCLUDE_DIRS :=$(subst $(PUBLIC_SW_PACKAGES_DIR),%PUBLIC_SW_PACKAGES_DIR%,$(ALL_INCLUDE_DIRS))
ALL_INCLUDE_DIRS :=$(subst $(EXTERNAL_SOURCE_ROOT_DIR),%EXTERNAL_SOURCE_ROOT_DIR%,$(ALL_INCLUDE_DIRS))
ALL_INCLUDE_DIRS :=$(subst $(PUBLIC_DRIVERS_DIR),%PUBLIC_DRIVERS_DIR%,$(ALL_INCLUDE_DIRS))
ALL_INCLUDE_DIRS :=$(subst $(APP_ROOT_DIR),%APP_ROOT_DIR%,$(ALL_INCLUDE_DIRS))

$(CURR_OBJ_DIR)/%.o: %.c $(HEADER_FILES_DEPS) $(APP_ROOT_DIR)/.config
	$(info .    Compiling $<)
	$(eval SRC_FILE := $(realpath $<))
	$(info .    Compiling $(SRC_FILE))
	$(SET_CC_ENV_VARS)& $(CC) $(GLOBAL_CFLAGS) $(CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $(CC_OUTPUT_FLAG_AND_FILE) $<
	
#	open line to create preproccesor file
#	$(CC) -E -P $(GLOBAL_CFLAGS) $(CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $< -o  $@.pre 

$(CURR_OBJ_DIR)/%.oo: %.cc $(HEADER_FILES_DEPS) $(APP_ROOT_DIR)/.config
	$(info .    Compiling $<)
	$(SET_CC_ENV_VARS)& $(CC) $(GLOBAL_CFLAGS) $(CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $(CC_OUTPUT_FLAG_AND_FILE) $<
#	open line to create preproccesor file
#	$(CC) -E -P $(GLOBAL_CFLAGS) $(CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $< -o  $@.pre 

$(CURR_OBJ_DIR)/%.oop: %.cpp $(HEADER_FILES_DEPS) $(APP_ROOT_DIR)/.config
	$(info .    Compiling $<)
	$(SET_CC_ENV_VARS)& $(CC) $(GLOBAL_CFLAGS) $(CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $(CC_OUTPUT_FLAG_AND_FILE) $<
#	open line to create preproccesor file
#	$(CC) -E -P $(GLOBAL_CFLAGS) $(CFLAGS) $(ALL_INCLUDE_DIRS) $(ALL_DEFINES) $< -o  $@.pre 
