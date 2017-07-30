ifdef CONFIG_REDEFINED_OUTPUT_NAME
    CONFIG_REDEFINED_OUTPUT_NAME :=$(patsubst "%",%,$(CONFIG_REDEFINED_OUTPUT_NAME))
    OUTPUT_NAME :=$(CONFIG_REDEFINED_OUTPUT_NAME)
    HISTORY_OUTPUT_NAME :=$(CONFIG_REDEFINED_OUTPUT_NAME)_$(MAIN_VERSION_STR).so
else
#    ifdef CONFIG_GCC_OUTPUT_TYPE_DYNAMIC_LIBRARY
#        OUTPUT_NAME :=lib$(FULL_PROJECT_NAME).so
#        HISTORY_OUTPUT_NAME :=lib$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR).so
#    else ifdef CONFIG_GCC_OUTPUT_TYPE_APPLICATION
        OUTPUT_NAME :=$(FULL_PROJECT_NAME).elf
        HISTORY_OUTPUT_NAME :=$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR).elf
#    else
#        $(error ---- unknown output type ----)
#    endif
endif

LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_NAME)
LINKER_HISTORY_OUTPUT :=$(OUT_DIR_HISTORY)/$(HISTORY_OUTPUT_NAME)
OUTPUT_ASM :=  $(OUT_DIR)/$(OUTPUT_NAME).asm
OUTPUT_BIN := $(OUT_DIR)/$(OUTPUT_NAME).bin
OUTPUT_HEX :=  $(OUT_DIR)/$(OUTPUT_NAME).hex

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    LINKER_OUTPUT := $(subst /,\,$(LINKER_OUTPUT))
    LINKER_HISTORY_OUTPUT := $(subst /,\,$(LINKER_HISTORY_OUTPUT))
    OUTPUT_ASM := $(subst /,\,$(OUTPUT_ASM))
    OUTPUT_BIN := $(subst /,\,$(OUTPUT_BIN))
    OUTPUT_HEX := $(subst /,\,$(OUTPUT_HEX))
    OUTPUT_CRC32 := $(subst /,\,$(OUTPUT_CRC32))
endif



#{{{{{{{{   LDFLAGS PREPARATIONS   {{{{{{{{

#init LDFLAGS
LDFLAGS :=

LDFLAGS += --xtensa-core=$(XCC_CORE)
LDFLAGS += --xtensa-system=$(CORE_CONFIG_DIR)
LDFLAGS += -Wl,-Map=$(OUT_DIR)/$(OUTPUT_NAME).map


LDFLAGS := $(GLOBAL_LDFLAGS) $(LDFLAGS)

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
LIBS := $(patsubst lib%,-l%,$(GLOBAL_LIBS))
LIBS := $(patsubst %.a,%,$(LIBS))


LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))

#}}}}}}}}  END OF LIBRARIES PREPARATIONS }}}}}}}}







#{{{{{{{{  LINKER SCRIPT FILE PREPARATIONS {{{{{{{{

#ifdef CONFIG_USE_APPLICATION_SPECIFIC_SCATTER_FILE
#    SCATTER_FILE =$(APP_ROOT_DIR)/$(PROJECT_NAME).lds
#    ifeq ($(wildcard $(SCATTER_FILE)),) #if scatter file not found
#        $(info !--- application configured to use it's own scatter file,)
#        $(info !--- but $(SCATTER_FILE) doesn't exist)
#        $(error )
#    endif
#else
##    SCATTER_FILES_DIR :=$(BUILD_TOOLS_ROOT_DIR)/scatter_files/arm
##    LDS_PREPROCESSOR_DEFS += -DFILES_TO_FORCE_IN_RAM="$(FILES_TO_FORCE_IN_RAM)"
##    ifdef CONFIG_CORTEX_M4
##        SCATTER_FILE_PATTERN =$(SCATTER_FILES_DIR)/arm_gcc_cortex_m.lds
##    else
##        SCATTER_FILE_PATTERN =$(SCATTER_FILES_DIR)/arm_gcc_cortex_a.lds
##    endif
##
##    SCATTER_FILE =$(OUT_DIR)/$(OUTPUT_APP_NAME).lds
#endif

#}}}}}}}}  END OF LINKER SCRIPT FILE PREPARATIONS }}}}}}}}




#{{{{{{{{  CREATING OBJECT LIST {{{{{{{{

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

ALL_OBJ_FILES = $(call rwildcard,$(OBJ_DIR)/,*.o)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.oo)
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




ifeq ($(findstring y,$(CONFIG_USED_FOR_SEMIHOSTING_UPLOADING)),y)
    CONFIG_CALCULATE_CRC32=y
endif




LINKER_CMD =$(LD) $(LDFLAGS)
LINKER_CMD += -Wl,@$(ALL_OBJECTS_LIST_FILE) 
LINKER_CMD += -o $(LINKER_OUTPUT)

LINKER_TO_ASM_CMD = $(XCC_ROOT_DIR)/bin/xt-objdump
LINKER_TO_ASM_CMD += --xtensa-core=$(XCC_CORE)
LINKER_TO_ASM_CMD += --xtensa-system=$(CORE_CONFIG_DIR)
LINKER_TO_ASM_CMD +=-d -S $(LINKER_OUTPUT) > $(OUTPUT_ASM)

export
#	$(CREATE_LDS_CMD)
#	$(FULL_GCC_PREFIX)objcopy -O ihex $(LINKER_OUTPUT) $(OUTPUT_HEX)
#	$(CP)  $(OUTPUT_BIN) $(OUTPUT_HISTORY_BIN)
#	$(FULL_GCC_PREFIX)objcopy -O binary $(LINKER_OUTPUT) $(OUTPUT_BIN)
build_outputs :
	$(LINKER_CMD)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
	$(LINKER_TO_ASM_CMD)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
ifdef POST_BUILD_MAKEFILE_DIR
	$(MAKE) -C $(POST_BUILD_MAKEFILE_DIR) -f Makefile.postbuild.mk build_outputs
endif
