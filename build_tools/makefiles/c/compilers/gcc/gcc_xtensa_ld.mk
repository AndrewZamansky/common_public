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
#        $(info err: unknown output type)
#        $(call exit,1)
#    endif
endif

LINKER_OUTPUT :=$(call fix_path_if_in_windows,$(OUT_DIR)/$(OUTPUT_NAME))
OUTPUT_BIN :=$(call fix_path_if_in_windows,$(OUT_DIR)/$(OUTPUT_NAME).bin)
OUTPUT_HEX :=$(call fix_path_if_in_windows,$(OUT_DIR)/$(OUTPUT_NAME).hex)
OUTPUT_ASM :=$(call fix_path_if_in_windows,$(OUT_DIR)/$(OUTPUT_NAME).asm)
OUTPUT_CRC32 :=$(call fix_path_if_in_windows,$(OUTPUT_CRC32))
LINKER_HISTORY_OUTPUT :=$(call fix_path_if_in_windows,\
                                    $(OUT_DIR_HISTORY)/$(HISTORY_OUTPUT_NAME))

#{{{{{{{{   LDFLAGS PREPARATIONS   {{{{{{{{

#init LDFLAGS
LDFLAGS :=

LDFLAGS += -nostdlib
LDFLAGS += -Wl,--no-check-sections
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-static
LDFLAGS += -u call_user_start
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

ifdef CONFIG_USE_APPLICATION_SPECIFIC_SCATTER_FILE
    SCATTER_FILE =$(APP_ROOT_DIR)/$(PROJECT_NAME).lds
    ifeq ($(wildcard $(SCATTER_FILE)),) #if scatter file not found
        $(info err: application configured to use it's own scatter file,)
        $(info ---: but $(SCATTER_FILE) doesn't exist)
        $(call exit,1)
    endif
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
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.o.asm)
ALL_OBJ_FILES += $(call rwildcard,$(OBJ_DIR)/,*.O.asm)
#some time, on windows, scan for .O.asm and .o.asm will generate duplicate files
#so $(sort) will eliminate duplication
ALL_OBJ_FILES :=$(sort $(ALL_OBJ_FILES))
ALL_OBJ_FILES :=$(subst \,/,$(ALL_OBJ_FILES))

ALL_OBJECTS_LIST_FILE:=$(OUT_DIR)\objects.txt
DUMMY := $(call fwrite,$(ALL_OBJECTS_LIST_FILE),$(ALL_OBJ_FILES),TRUNCATE)

#}}}}}}}}  END OF CREATING OBJECT LIST  }}}}}}}}




ifeq ($(findstring y,$(CONFIG_USED_FOR_SEMIHOSTING_UPLOADING)),y)
    CONFIG_CALCULATE_CRC32=y
endif


ifdef CONFIG_USE_APPLICATION_SPECIFIC_SCATTER_FILE
    CREATE_LDS_CMD += echo using application specifc scatter file
else
    CREATE_LDS_CMD =$(CC) -E -P -x c -I z_auto_generated_files
    CREATE_LDS_CMD += $(LDS_PREPROCESSOR_DEFS)
    CREATE_LDS_CMD += $(SCATTER_FILE_PATTERN) -o $(SCATTER_FILE)
endif

LINKER_CMD =$(LD) $(LDFLAGS) -T $(SCATTER_FILE) $(LIBRARIES_DIRS)
LINKER_CMD += -Wl,--start-group
LINKER_CMD += $(LIBS) @$(ALL_OBJECTS_LIST_FILE) 
LINKER_CMD += -Wl,--end-group
LINKER_CMD += -o $(LINKER_OUTPUT)

export
#	$(FULL_GCC_PREFIX)objcopy -O ihex $(LINKER_OUTPUT) $(OUTPUT_HEX)
#	$(CP)  $(OUTPUT_BIN) $(OUTPUT_HISTORY_BIN)
#	$(FULL_GCC_PREFIX)objcopy -O binary $(LINKER_OUTPUT) $(OUTPUT_BIN)
build_outputs :
	$(CREATE_LDS_CMD)
	$(LINKER_CMD)
	$(FULL_GCC_PREFIX)objdump -d -S $(LINKER_OUTPUT) > $(OUTPUT_ASM)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
ifeq ($(findstring y,$(CONFIG_USED_FOR_SEMIHOSTING_UPLOADING)),y)
	$(CP)  $(OUTPUT_BIN) $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
	$(CP) $(OUTPUT_CRC32) $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
endif
