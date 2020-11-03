ifdef CONFIG_REDEFINED_OUTPUT_NAME
    OUTPUT_NAME :=$(patsubst "%",%,$(CONFIG_REDEFINED_OUTPUT_NAME))
    HISTORY_OUTPUT_NAME :=$(OUTPUT_NAME)_$(MAIN_VERSION_STR)
    HISTORY_OUTPUT_NAME :=$(HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR)
else
    ifdef CONFIG_OUTPUT_TYPE_STATIC_LIBRARY
        OUTPUT_NAME :=lib$(FULL_PROJECT_NAME).a
        HISTORY_OUTPUT_NAME :=lib$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR)
        HISTORY_OUTPUT_NAME :=$(HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR).a
    else ifdef CONFIG_OUTPUT_TYPE_APPLICATION
        OUTPUT_NAME :=$(FULL_PROJECT_NAME).elf
        HISTORY_OUTPUT_NAME :=$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR)
        HISTORY_OUTPUT_NAME :=$(HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR).elf
    else
        $(info err: unknown output type)
        $(call exit,1)
    endif
endif

LINKER_OUTPUT :=$(call fix_path_if_in_windows,$(OUT_DIR)/$(OUTPUT_NAME))
LINKER_HISTORY_OUTPUT :=$(call fix_path_if_in_windows,\
                             $(OUT_DIR_HISTORY)/$(HISTORY_OUTPUT_NAME))
OUTPUT_ASM :=$(call fix_path_if_in_windows,$(OUT_DIR)/$(OUTPUT_NAME).asm)
OUTPUT_BIN :=$(call fix_path_if_in_windows,$(OUT_DIR)/$(OUTPUT_NAME).bin)
OUTPUT_HEX :=$(call fix_path_if_in_windows,$(OUT_DIR)/$(OUTPUT_NAME).ihex.hex)
OUTPUT_SREC_HEX :=$(call\
         fix_path_if_in_windows,$(OUT_DIR)/$(OUTPUT_NAME).srec.hex)
OUTPUT_CRC32 :=$(call fix_path_if_in_windows,$(OUTPUT_CRC32))

#{{{{{{{{   LDFLAGS PREPARATIONS   {{{{{{{{

#init LDFLAGS
LDFLAGS := 

LDFLAGS += --xtensa-core=$(XCC_CORE)
LDFLAGS += --xtensa-system=$(CORE_CONFIG_DIR)
LDFLAGS += -Wl,-Map=$(OUT_DIR)/$(OUTPUT_NAME).map

GLOBAL_LDFLAGS := $(sort $(GLOBAL_LDFLAGS))# remove duplicates
LDFLAGS := $(GLOBAL_LDFLAGS) $(LDFLAGS)

ifndef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
    LDFLAGS += -nostdlib
endif

#}}}}}}}}  END OF LDFLAGS PREPARATIONS }}}}}}}}




#{{{{{{{{   LIBRARIES PREPARATIONS {{{{{{{{

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
    ifdef CONFIG_USE_NANO_STD_LIBS
        STD_LIBRARIES := libc_nano.a
    else
        STD_LIBRARIES := libc.a
    endif
    STD_LIBRARIES +=libm.a libgcc.a
    GLOBAL_LIBS += $(STD_LIBRARIES)
    ifdef CONFIG_TOOLCHAIN_LIBRARIES_ARE_SPEED_CRUCIAL
        SPEED_CRITICAL_STD_LIBS := $(STD_LIBRARIES)
    endif
endif
GLOBAL_LIBS :=$(sort $(GLOBAL_LIBS)) #remove duplicates
LIBS := $(patsubst lib%,-l%,$(GLOBAL_LIBS))
LIBS := $(patsubst %.a,%,$(LIBS))

WHOLE_LIBS := $(patsubst lib%,-l%,$(GLOBAL_WHOLE_LIBS))
WHOLE_LIBS := $(patsubst %.a,%,$(WHOLE_LIBS))

LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))

#}}}}}}}}  END OF LIBRARIES PREPARATIONS }}}}}}}}







#{{{{{{{{  LINKER SCRIPT FILE PREPARATIONS {{{{{{{{
ifdef CONFIG_USE_APPLICATION_SPECIFIC_SCATTER_FILE
    SCATTER_FILE =$(APP_ROOT_DIR)/$(PROJECT_NAME).lds
    ifeq ($(wildcard $(SCATTER_FILE)),) #if scatter file not found
        $(info err: application configured to use it's own scatter file,)
        $(info ---: but $(SCATTER_FILE) doesn't exist)
        $(call exit,1)
    endif
    LDFLAGS += -T $(SCATTER_FILE)
else
#    SCATTER_FILES_DIR :=$(BUILD_TOOLS_ROOT_DIR)/scatter_files/arm
#    LDS_PREPROCESSOR_DEFS += -DFILES_TO_FORCE_IN_RAM="$(FILES_TO_FORCE_IN_RAM)"
#    ifdef CONFIG_CORTEX_M4
#        SCATTER_FILE_PATTERN =$(SCATTER_FILES_DIR)/arm_gcc_cortex_m.lds
#    else
#        SCATTER_FILE_PATTERN =$(SCATTER_FILES_DIR)/arm_gcc_cortex_a.lds
#    endif
#
#    SCATTER_FILE =$(OUT_DIR)/$(OUTPUT_APP_NAME).lds
endif

#}}}}}}}}  END OF LINKER SCRIPT FILE PREPARATIONS }}}}}}}}




#{{{{{{{{  CREATING OBJECT LIST {{{{{{{{

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

ALL_OBJ_FILES = $(call rwildcard,$(OBJ_DIR)/,*.o)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.oo)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.oop)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.o.asm)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.O.asm)
#some time, on windows, scan for .O.asm and .o.asm will generate duplicate files
#so $(sort) will eliminate duplication
ALL_OBJ_FILES :=$(sort $(ALL_OBJ_FILES))
ALL_OBJ_FILES :=$(subst \,/,$(ALL_OBJ_FILES))

ALL_OBJECTS_LIST_FILE:=$(OUT_DIR)\objects.txt

#create file with list of objects
LIST_FILE_NAME_TRUNCATE :=$(ALL_OBJECTS_LIST_FILE)
PREFIX_FOR_EACH_ITEM :=
ITEMS := $(ALL_OBJ_FILES)
include $(MAKEFILES_INC_FUNC_DIR)/add_item_list_to_file.mk
#end of file creation

#}}}}}}}}  END OF CREATING OBJECT LIST  }}}}}}}}



ifdef XCC_LINKER_SUPPORT_PACKAGE_DIR
    LDFLAGS += -mlsp=$(XCC_LINKER_SUPPORT_PACKAGE_DIR)
endif

ifeq ($(findstring y,$(CONFIG_USED_FOR_SEMIHOSTING_UPLOADING)),y)
    CONFIG_CALCULATE_CRC32=y
endif

ifdef CONFIG_OUTPUT_TYPE_STATIC_LIBRARY
    LINKER_CMD =$(AR) r $(LINKER_OUTPUT) @$(ALL_OBJECTS_LIST_FILE)
else ifdef CONFIG_OUTPUT_TYPE_APPLICATION
    LINKER_CMD_ARG :=$(LDFLAGS) $(LIBRARIES_DIRS) 
    LINKER_CMD_ARG += -Wl,--gc-sections
    LINKER_CMD_ARG += -Wl,@$(ALL_OBJECTS_LIST_FILE)  $(LIBS)
    ifneq ($(strip $(WHOLE_LIBS)),)
        LINKER_CMD_ARG += -Wl,--whole-archive $(WHOLE_LIBS) -Wl,--no-whole-archive
    endif
    LINKER_CMD_ARG += -o $(LINKER_OUTPUT)
    LINKER_CMD_ARG :=$(subst \,/,$(LINKER_CMD_ARG))# ipa receives only '/'
    LINKER_CMD :=cd /D $(OUT_DIR)/ & $(LD) $(LINKER_CMD_ARG)
endif


LINKER_TO_ASM_CMD = $(XCC_ROOT_DIR)/bin/xt-objdump
LINKER_TO_ASM_CMD += --xtensa-core=$(XCC_CORE)
LINKER_TO_ASM_CMD += --xtensa-system=$(CORE_CONFIG_DIR)
LINKER_TO_ASM_CMD +=-d -S $(LINKER_OUTPUT) > $(OUTPUT_ASM)

LINKER_TO_HEX_CMD = $(XCC_ROOT_DIR)/bin/xt-objcopy
LINKER_TO_HEX_CMD += --xtensa-core=$(XCC_CORE)
LINKER_TO_HEX_CMD += --xtensa-system=$(CORE_CONFIG_DIR)
LINKER_TO_HEX_CMD += -O ihex $(LINKER_OUTPUT) $(OUTPUT_HEX)

LINKER_TO_SREC_HEX_CMD = $(XCC_ROOT_DIR)/bin/xt-objcopy
LINKER_TO_SREC_HEX_CMD += --xtensa-core=$(XCC_CORE)
LINKER_TO_SREC_HEX_CMD += --xtensa-system=$(CORE_CONFIG_DIR)
LINKER_TO_SREC_HEX_CMD += -O srec $(LINKER_OUTPUT) $(OUTPUT_SREC_HEX)

LINKER_TO_BIN_CMD = $(XCC_ROOT_DIR)/bin/xt-objcopy
LINKER_TO_BIN_CMD += --xtensa-core=$(XCC_CORE)
LINKER_TO_BIN_CMD += --xtensa-system=$(CORE_CONFIG_DIR)
LINKER_TO_BIN_CMD += -O binary $(LINKER_OUTPUT) $(OUTPUT_BIN)

export
#	$(CREATE_LDS_CMD)

build_outputs :
	$(LINKER_CMD)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
	$(LINKER_TO_ASM_CMD)
ifneq ($(findstring y,$(CONFIG_OUTPUT_TYPE_STATIC_LIBRARY)),y)
	$(LINKER_TO_HEX_CMD)
	$(LINKER_TO_SREC_HEX_CMD)
	$(LINKER_TO_BIN_CMD)
endif
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif

