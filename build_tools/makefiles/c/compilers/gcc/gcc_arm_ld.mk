ifdef CONFIG_REDEFINED_OUTPUT_NAME

    OUTPUT_NAME :=$(patsubst "%",%,$(CONFIG_REDEFINED_OUTPUT_NAME))

    _HISTORY_OUTPUT_NAME :=$(OUTPUT_NAME)_$(MAIN_VERSION_STR)
    HISTORY_OUTPUT_NAME :=$(_HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR)
    HISTORY_OUTPUT_NAME_NO_DBG :=\
        $(_HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR).noDBG

else
#    ifdef CONFIG_OUTPUT_TYPE_DYNAMIC_LIBRARY
#        OUTPUT_NAME :=lib$(FULL_PROJECT_NAME).so
#        HISTORY_OUTPUT_NAME :=lib$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR).so
    ifdef CONFIG_OUTPUT_TYPE_STATIC_LIBRARY
        OUTPUT_NAME :=lib$(FULL_PROJECT_NAME).a
        _HISTORY_OUTPUT_NAME :=lib$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR)
        HISTORY_OUTPUT_NAME :=$(_HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR).a
        HISTORY_OUTPUT_NAME_NO_DBG :=\
             $(_HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR).noDBG.a
    else ifdef CONFIG_OUTPUT_TYPE_APPLICATION
        OUTPUT_NAME :=$(FULL_PROJECT_NAME).elf
        _HISTORY_OUTPUT_NAME :=$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR)
        HISTORY_OUTPUT_NAME :=\
             $(_HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR).elf
        HISTORY_OUTPUT_NAME_NO_DBG :=\
             $(_HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR).noDBG.elf
    else
        $(info err: unknown output type)
        $(call exit,1)
    endif
endif

LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_NAME)
MAP_FILE := $(OUT_DIR)/$(OUTPUT_NAME).map
LINKER_HISTORY_OUTPUT :=$(OUT_DIR_HISTORY)/$(HISTORY_OUTPUT_NAME)
LINKER_HISTORY_noDBG_OUTPUT :=$(call fix_path_if_in_windows,\
                             $(OUT_DIR_HISTORY)/$(HISTORY_OUTPUT_NAME_NO_DBG))
OUTPUT_ASM :=  $(OUT_DIR)/$(OUTPUT_NAME).asm
OUTPUT_BIN := $(OUT_DIR)/$(OUTPUT_NAME).bin
OUTPUT_HEX :=  $(OUT_DIR)/$(OUTPUT_NAME).hex

#{{{{{{{{  LINKER SCRIPT FILE PREPARATIONS {{{{{{{{

ifdef CONFIG_USE_APPLICATION_SPECIFIC_SCATTER_FILE
    SCATTER_FILE =$(APP_ROOT_DIR)/$(PROJECT_NAME).lds
    ifeq ($(wildcard $(SCATTER_FILE)),) #if scatter file not found
        $(info err: application configured to use it's own scatter file,)
        $(info ---: but $(SCATTER_FILE) doesn't exist)
        $(call exit,1)
    endif
else
    SCATTER_FILES_DIR :=$(BUILD_TOOLS_ROOT_DIR)/scatter_files/arm
    LDS_PREPROCESSOR_DEFS += -DFILES_TO_FORCE_IN_RAM="$(FILES_TO_FORCE_IN_RAM)"
    SCATTER_FILE_PATTERN =$(SCATTER_FILES_DIR)/arm_gcc_cortex.lds

    SCATTER_FILE =$(OUT_DIR)/$(OUTPUT_NAME).lds
endif

#}}}}}}}}  END OF LINKER SCRIPT FILE PREPARATIONS }}}}}}}}


SCATTER_FILE :=$(call fix_path_if_in_windows,$(SCATTER_FILE))
LINKER_OUTPUT :=$(call fix_path_if_in_windows,$(LINKER_OUTPUT))
MAP_FILE :=$(call fix_path_if_in_windows,$(MAP_FILE))
OUTPUT_BIN :=$(call fix_path_if_in_windows,$(OUTPUT_BIN))
OUTPUT_ASM :=$(call fix_path_if_in_windows,$(OUTPUT_ASM))
OUTPUT_HEX :=$(call fix_path_if_in_windows,$(OUTPUT_HEX))
OUTPUT_CRC32 :=$(call fix_path_if_in_windows,$(OUTPUT_CRC32))
LINKER_HISTORY_OUTPUT :=$(call fix_path_if_in_windows,$(LINKER_HISTORY_OUTPUT))

#{{{{{{{{   LDFLAGS PREPARATIONS   {{{{{{{{

#init LDFLAGS
LDFLAGS :=

ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
    LDFLAGS += -mthumb
endif

LDFLAGS += -mcpu=$(CONFIG_CPU_TYPE)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -nostartfiles
LDFLAGS += -Wl,-Map=$(MAP_FILE)
ifndef CONFIG_CORTEX_A35
    LDFLAGS += -mthumb-interwork
endif

ifdef CONFIG_USE_NANO_STD_LIBS
    LDFLAGS += -specs=nano.specs
endif


ifdef CONFIG_CORTEX_M4
    ifdef CONFIG_INCLUDE_CORTEX_M_FPU
        LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
    else
        LDFLAGS += -mfloat-abi=soft
    endif
endif


LDFLAGS += -g -g3 -ggdb3 #-gstabs3  #-O0

LDFLAGS := $(GLOBAL_LDFLAGS) $(LDFLAGS)

#}}}}}}}}  END OF LDFLAGS PREPARATIONS }}}}}}}}




#{{{{{{{{   LIBRARIES PREPARATIONS {{{{{{{{

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
    ifdef CONFIG_USE_NANO_STD_LIBS
        STD_LIBRARIES := libc_nano.a
    else
        STD_LIBRARIES := libc.a libstdc++.a
    endif
    STD_LIBRARIES +=libm.a libgcc.a
    GLOBAL_LIBS += $(STD_LIBRARIES)
    ifdef CONFIG_TOOLCHAIN_LIBRARIES_ARE_SPEED_CRUCIAL
        SPEED_CRITICAL_STD_LIBS := $(STD_LIBRARIES)
    endif
endif
LIBS := $(patsubst lib%,-l%,$(GLOBAL_LIBS))
LIBS := $(patsubst %.a,%,$(LIBS))

WHOLE_LIBS := $(patsubst lib%,-l%,$(GLOBAL_WHOLE_LIBS))
WHOLE_LIBS := $(patsubst %.a,%,$(WHOLE_LIBS))

ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
#	GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(GCC_LIB_ROOT_DIR)/fpu
#	GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(GCC_LIB_ROOT_DIR)/thumb
else
    GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(GCC_LIB_ROOT_DIR)
endif
LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))

#}}}}}}}}  END OF LIBRARIES PREPARATIONS }}}}}}}}




#{{{{{{{{   CODE TO PUT IN RAM  {{{{{{{{
ALL_RAM_CONFIGS :=$(SPEED_CRITICAL_CODE_LOCATION_INTERNAL_SRAM)
ALL_RAM_CONFIGS +=$(SPEED_CRITICAL_CODE_LOCATION_DDR)
ALL_RAM_CONFIGS :=$(sort $(ALL_RAM_OPTIONS))
ifeq ($(ALL_RAM_CONFIGS),y)
    FILES_TO_FORCE_IN_RAM := $(SPEED_CRITICAL_FILES) $(SPEED_CRITICAL_STD_LIBS)
    FILES_TO_FORCE_IN_RAM := $(sort $(FILES_TO_FORCE_IN_RAM))
    FILES_TO_FORCE_IN_RAM := $(patsubst %.c,%.o,$(FILES_TO_FORCE_IN_RAM))
    FILES_TO_FORCE_IN_RAM := $(patsubst %.cc,%.oo,$(FILES_TO_FORCE_IN_RAM))
    FILES_TO_FORCE_IN_RAM := $(patsubst %.cpp,%.oop,$(FILES_TO_FORCE_IN_RAM))
    FILES_TO_FORCE_IN_RAM := $(patsubst %.s,%.o.asm,$(FILES_TO_FORCE_IN_RAM))
    FILES_TO_FORCE_IN_RAM := $(patsubst %.S,%.O.asm,$(FILES_TO_FORCE_IN_RAM))
    # add * to deal with libraries pathes  :
    FILES_TO_FORCE_IN_RAM := $(patsubst %,*%,$(FILES_TO_FORCE_IN_RAM))
else
    FILES_TO_FORCE_IN_RAM :=
endif
# add dummy file to create non-empy field in lds file for correct syntax :
FILES_TO_FORCE_IN_RAM += 123_DUMMY.X

#}}}}}}}}   END OF CODE TO PUT IN RAM  }}}}}}}}






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

ALL_OBJECTS_LIST_FILE:=$(OUT_DIR)/objects.txt
DUMMY := $(call fwrite,$(ALL_OBJECTS_LIST_FILE),$(ALL_OBJ_FILES),TRUNCATE)

#}}}}}}}}  END OF CREATING OBJECT LIST  }}}}}}}}





ifdef CONFIG_USE_APPLICATION_SPECIFIC_SCATTER_FILE
    CREATE_LDS_CMD += echo using application specifc scatter file
else ifdef CONFIG_OUTPUT_TYPE_STATIC_LIBRARY
    CREATE_LDS_CMD += echo building library, no need in scatter file
else
    FMT_GLOBAL_DEFINES := $(patsubst %,-D%,$(GLOBAL_DEFINES))
    #substitute " to \" for string defines  :
    FMT_GLOBAL_DEFINES := $(subst ",\", $(FMT_GLOBAL_DEFINES))

    FMT_GLOBAL_INCLUDE_DIR := $(patsubst %,-I%,$(GLOBAL_INCLUDE_DIR))

    CREATE_LDS_CMD =$(CC) -E -P -x c $(FMT_GLOBAL_DEFINES)
    CREATE_LDS_CMD += $(LDS_PREPROCESSOR_DEFS)

    CREATE_LDS_CMD_TEST := $(CREATE_LDS_CMD) $(FMT_GLOBAL_INCLUDE_DIR)
    CREATE_LDS_CMD_TEST += $(SCATTER_FILE_PATTERN) -o $(SCATTER_FILE)

    LONG_LDS_CMD:=$(call check_win_cmd_len,$(CREATE_LDS_CMD_TEST))
    ifeq ($(LONG_LDS_CMD),TOO_LONG)
        LDS_ARGS_CONTENT :=$(subst \,/,$(FMT_GLOBAL_INCLUDE_DIR))
        LDS_ARGS_FILE :=$(OUT_DIR)/lds.args
        DUMMY := $(call \
             fwrite,$(LDS_ARGS_FILE),$(LDS_ARGS_CONTENT),TRUNCATE)
        CREATE_LDS_CMD += $(CC_USE_ARGS_FROM_FILE_FLAG)$(LDS_ARGS_FILE)
        CREATE_LDS_CMD += $(SCATTER_FILE_PATTERN) -o $(SCATTER_FILE)
    else
        CREATE_LDS_CMD :=$(CREATE_LDS_CMD_TEST)
    endif
endif

AR := $(FULL_GCC_PREFIX)ar
ifdef CONFIG_OUTPUT_TYPE_STATIC_LIBRARY
    LINKER_CMD = $(AR) cr -o $(LINKER_OUTPUT) @$(ALL_OBJECTS_LIST_FILE)
else
    LINKER_CMD =$(LD) $(LDFLAGS) -T $(SCATTER_FILE) $(LIBRARIES_DIRS)
    LINKER_CMD += @$(ALL_OBJECTS_LIST_FILE)
    ifneq ($(strip $(WHOLE_LIBS)),)
        LINKER_CMD += -Wl,--whole-archive $(WHOLE_LIBS) -Wl,--no-whole-archive
    endif
    LINKER_CMD += $(LIBS) -o $(LINKER_OUTPUT)
endif

LINKER_TO_noDBG_CMD = $(FULL_GCC_PREFIX)strip
LINKER_TO_noDBG_CMD += --strip-debug
LINKER_TO_noDBG_CMD += -o $(LINKER_HISTORY_noDBG_OUTPUT)
LINKER_TO_noDBG_CMD += $(LINKER_HISTORY_OUTPUT)


build_outputs :
	$(CREATE_LDS_CMD)
	$(LINKER_CMD)
	$(FULL_GCC_PREFIX)objdump -d -S $(LINKER_OUTPUT) > $(OUTPUT_ASM)
ifneq ($(findstring y,$(CONFIG_OUTPUT_TYPE_STATIC_LIBRARY)),y)
	$(FULL_GCC_PREFIX)objcopy -O binary $(LINKER_OUTPUT) $(OUTPUT_BIN)
	$(FULL_GCC_PREFIX)objcopy -O ihex $(LINKER_OUTPUT) $(OUTPUT_HEX)
endif
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_PRODUCE_OUTPUT_WITHOUT_DEBUG_INFO)),y)
	$(LINKER_TO_noDBG_CMD)
endif	
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
ifeq ($(findstring y,$(CONFIG_USED_FOR_SEMIHOSTING_UPLOADING)),y)
	$(CP)  $(OUTPUT_BIN) $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
	$(CP) $(OUTPUT_CRC32) $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
endif
