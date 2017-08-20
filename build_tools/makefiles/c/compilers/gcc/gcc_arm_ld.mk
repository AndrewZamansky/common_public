ifdef CONFIG_REDEFINED_OUTPUT_NAME
    CONFIG_REDEFINED_OUTPUT_NAME :=$(patsubst "%",%,$(CONFIG_REDEFINED_OUTPUT_NAME))
    OUTPUT_NAME :=$(CONFIG_REDEFINED_OUTPUT_NAME)
    HISTORY_OUTPUT_NAME :=$(CONFIG_REDEFINED_OUTPUT_NAME)_$(MAIN_VERSION_STR).so
    OUTPUT_HISTORY_BIN_NAME :=$(CONFIG_REDEFINED_OUTPUT_NAME)_$(MAIN_VERSION_STR)
else
#    ifdef CONFIG_GCC_OUTPUT_TYPE_DYNAMIC_LIBRARY
#        OUTPUT_NAME :=lib$(FULL_PROJECT_NAME).so
#        HISTORY_OUTPUT_NAME :=lib$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR).so
#    else ifdef CONFIG_GCC_OUTPUT_TYPE_APPLICATION
        OUTPUT_NAME :=$(FULL_PROJECT_NAME).elf
        HISTORY_OUTPUT_NAME :=$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR).elf
        OUTPUT_HISTORY_BIN_NAME :=$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR)
#    else
#        $(error ---- unknown output type ----)
#    endif
endif

LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_NAME)
MAP_FILE := $(OUT_DIR)/$(OUTPUT_NAME).map
LINKER_HISTORY_OUTPUT :=$(OUT_DIR_HISTORY)/$(HISTORY_OUTPUT_NAME)
OUTPUT_HISTORY_BIN :=$(OUT_DIR_HISTORY)/$(OUTPUT_HISTORY_BIN_NAME)r$(DATE_STR).bin
OUTPUT_ASM :=  $(OUT_DIR)/$(OUTPUT_NAME).asm
OUTPUT_BIN := $(OUT_DIR)/$(OUTPUT_NAME).bin
OUTPUT_HEX :=  $(OUT_DIR)/$(OUTPUT_NAME).hex

#{{{{{{{{  LINKER SCRIPT FILE PREPARATIONS {{{{{{{{

ifdef CONFIG_USE_APPLICATION_SPECIFIC_SCATTER_FILE
    SCATTER_FILE =$(APP_ROOT_DIR)/$(PROJECT_NAME).lds
    ifeq ($(wildcard $(SCATTER_FILE)),) #if scatter file not found
        $(info !--- application configured to use it's own scatter file,)
        $(info !--- but $(SCATTER_FILE) doesn't exist)
        $(error )
    endif
else
    SCATTER_FILES_DIR :=$(BUILD_TOOLS_ROOT_DIR)/scatter_files/arm
    LDS_PREPROCESSOR_DEFS += -DFILES_TO_FORCE_IN_RAM="$(FILES_TO_FORCE_IN_RAM)"
    ifdef CONFIG_CORTEX_M4
        SCATTER_FILE_PATTERN =$(SCATTER_FILES_DIR)/arm_gcc_cortex_m.lds
    else
        SCATTER_FILE_PATTERN =$(SCATTER_FILES_DIR)/arm_gcc_cortex_a.lds
    endif

    SCATTER_FILE =$(OUT_DIR)/$(OUTPUT_NAME).lds
endif

#}}}}}}}}  END OF LINKER SCRIPT FILE PREPARATIONS }}}}}}}}

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    SCATTER_FILE := $(subst /,\,$(SCATTER_FILE))
    LINKER_OUTPUT := $(subst /,\,$(LINKER_OUTPUT))
    MAP_FILE := $(subst /,\,$(MAP_FILE))
    LINKER_HISTORY_OUTPUT := $(subst /,\,$(LINKER_HISTORY_OUTPUT))
    OUTPUT_HISTORY_BIN := $(subst /,\,$(OUTPUT_HISTORY_BIN))
    OUTPUT_ASM := $(subst /,\,$(OUTPUT_ASM))
    OUTPUT_BIN := $(subst /,\,$(OUTPUT_BIN))
    OUTPUT_HEX := $(subst /,\,$(OUTPUT_HEX))
    OUTPUT_CRC32 := $(subst /,\,$(OUTPUT_CRC32))
endif



#{{{{{{{{   LDFLAGS PREPARATIONS   {{{{{{{{

#init LDFLAGS
LDFLAGS :=

ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
    LDFLAGS += -mthumb
endif

LDFLAGS += -mcpu=$(CONFIG_CPU_TYPE)
LDFLAGS += -mthumb-interwork
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -nostartfiles
LDFLAGS += -Wl,-Map=$(MAP_FILE)

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

ifeq ($(findstring cortex-m,$(CONFIG_CPU_TYPE)),cortex-m)
#	GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(GCC_LIB_ROOT_DIR)/fpu
#	GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(GCC_LIB_ROOT_DIR)/thumb
else	
    GLOBAL_LIBS_PATH := $(GLOBAL_LIBS_PATH) $(GCC_LIB_ROOT_DIR)
endif
LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))

#}}}}}}}}  END OF LIBRARIES PREPARATIONS }}}}}}}}




#{{{{{{{{   CODE TO PUT IN RAM  {{{{{{{{

ifdef CONFIG_PUT_SPEED_CRITICAL_CODE_TO_RAM
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

ALL_OBJECTS_LIST_FILE:=$(OUT_DIR)\objects.txt

#create file with list of objects
LIST_FILE_NAME_TRUNCATE :=$(ALL_OBJECTS_LIST_FILE)
PREFIX_FOR_EACH_ITEM :=
ITEMS := $(ALL_OBJ_FILES)
include $(MAKEFILES_INC_FUNC_DIR)/add_item_list_to_file_in_one_line.mk
#end of file creation

#}}}}}}}}  END OF CREATING OBJECT LIST  }}}}}}}}





ifdef CONFIG_USE_APPLICATION_SPECIFIC_SCATTER_FILE
    CREATE_LDS_CMD += echo using application specifc scatter file
else
    CREATE_LDS_CMD =$(CC) -E -P -x c -I z_auto_generated_files
    CREATE_LDS_CMD += $(LDS_PREPROCESSOR_DEFS)
    CREATE_LDS_CMD += $(SCATTER_FILE_PATTERN) -o $(SCATTER_FILE)
endif

LINKER_CMD =$(LD) $(LDFLAGS) -T $(SCATTER_FILE) $(LIBRARIES_DIRS)
LINKER_CMD += @$(ALL_OBJECTS_LIST_FILE) $(LIBS) -o $(LINKER_OUTPUT)

build_outputs :
	$(CREATE_LDS_CMD)
	$(LINKER_CMD)
	$(FULL_GCC_PREFIX)objdump -d -S $(LINKER_OUTPUT) > $(OUTPUT_ASM)
	$(FULL_GCC_PREFIX)objcopy -O binary $(LINKER_OUTPUT) $(OUTPUT_BIN)
	$(FULL_GCC_PREFIX)objcopy -O ihex $(LINKER_OUTPUT) $(OUTPUT_HEX)
	$(CP)  $(OUTPUT_BIN) $(OUTPUT_HISTORY_BIN)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
ifeq ($(findstring y,$(CONFIG_USED_FOR_SEMIHOSTING_UPLOADING)),y)
	$(CP)  $(OUTPUT_BIN) $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
	$(CP) $(OUTPUT_CRC32) $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
endif
