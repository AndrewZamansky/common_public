ifdef CONFIG_REDEFINED_OUTPUT_NAME

    OUTPUT_NAME :=$(patsubst "%",%,$(CONFIG_REDEFINED_OUTPUT_NAME))

    HISTORY_OUTPUT_NAME :=$(OUTPUT_NAME)_$(MAIN_VERSION_STR)
    HISTORY_OUTPUT_NAME :=$(HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR)

else
    ifdef CONFIG_OUTPUT_TYPE_STATIC_LIBRARY
        OUTPUT_NAME :=$(FULL_PROJECT_NAME).lib
        HISTORY_OUTPUT_NAME :=$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR)
        HISTORY_OUTPUT_NAME :=$(HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR).lib
    else ifdef OUTPUT_TYPE_APPLICATION
        OUTPUT_NAME :=$(FULL_PROJECT_NAME).axf
        HISTORY_OUTPUT_NAME :=$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR)
        HISTORY_OUTPUT_NAME :=$(HISTORY_OUTPUT_NAME)$(REVISION_FOR_FILE_STR).axf
    else
        $(info err: unknown output type)
        $(call exit,1)
    endif
endif

LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_NAME)
MAP_FILE := $(OUT_DIR)/$(OUTPUT_NAME).map
LINKER_HISTORY_OUTPUT :=$(OUT_DIR_HISTORY)/$(HISTORY_OUTPUT_NAME)
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
    SCATTER_FILE_PATTERN =$(SCATTER_FILES_DIR)/armcc_scatter_file_pattern.lds

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

ifdef CONFIG_CORTEX_M4
    ifdef CONFIG_INCLUDE_CORTEX_M_FPU
        LDFLAGS += --cpu=Cortex-M4.fp
    else 	 
        LDFLAGS += --cpu=Cortex-M4
    endif
endif


LDFLAGS += --library_type=microlib --strict 
LDFLAGS += --map --datacompressor=off --info=inline --entry do_startup 
LDFLAGS += --summary_stderr --info summarysizes
LDFLAGS += --info sizes --info totals 
LDFLAGS += --list $(OUT_DIR)/$(OUTPUT_NAME).map


LDFLAGS := $(GLOBAL_LDFLAGS) $(LDFLAGS)

#}}}}}}}}  END OF LDFLAGS PREPARATIONS }}}}}}}}




#{{{{{{{{   LIBRARIES PREPARATIONS {{{{{{{{

#TODO : see example in gcc_arm_ld.mk

#}}}}}}}}  END OF LIBRARIES PREPARATIONS }}}}}}}}




#{{{{{{{{   CODE TO PUT IN RAM  {{{{{{{{

#TODO : see example in gcc_arm_ld.mk

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

# armar syntax requires library output to be the first file in list
ifdef CONFIG_OUTPUT_TYPE_STATIC_LIBRARY
    ALL_OBJ_FILES := $(LINKER_OUTPUT) $(ALL_OBJ_FILES)
endif

ALL_OBJECTS_LIST_FILE:=$(OUT_DIR)/objects.txt

#create file with list of objects
LIST_FILE_NAME_TRUNCATE :=$(ALL_OBJECTS_LIST_FILE)
PREFIX_FOR_EACH_ITEM :=
ITEMS := $(ALL_OBJ_FILES)
include $(MAKEFILES_INC_FUNC_DIR)/add_item_list_to_file_in_one_line.mk
#end of file creation

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

    CREATE_LDS_CMD =$(CC) -E -P $(FMT_GLOBAL_INCLUDE_DIR)
    CREATE_LDS_CMD += $(FMT_GLOBAL_DEFINES)
    CREATE_LDS_CMD += $(LDS_PREPROCESSOR_DEFS)
    CREATE_LDS_CMD += $(SCATTER_FILE_PATTERN) -o $(SCATTER_FILE)
endif
CREATE_LDS_CMD :=$(call reduce_cmd_len, $(CREATE_LDS_CMD))
$(call check_win_cmd_len, $(CREATE_LDS_CMD))

ifdef CONFIG_OUTPUT_TYPE_STATIC_LIBRARY
    LINKER_CMD = $(ARMCC_ROOT_DIR)/bin/armar -r --via $(ALL_OBJECTS_LIST_FILE)
else
    LINKER_CMD = $(ARMCC_ROOT_DIR)/bin/armlink $(LDFLAGS)
    LINKER_CMD += --scatter $(SCATTER_FILE)
    LINKER_CMD += --via $(ALL_OBJECTS_LIST_FILE) -o $(LINKER_OUTPUT)
endif
$(call check_win_cmd_len, $(LINKER_CMD))

build_outputs :
	$(CREATE_LDS_CMD)
	$(LINKER_CMD)
	$(ARMCC_ROOT_DIR)/bin/fromelf -c $(LINKER_OUTPUT) --output $(OUTPUT_ASM)
ifneq ($(findstring y,$(CONFIG_OUTPUT_TYPE_STATIC_LIBRARY)),y)
	$(ARMCC_ROOT_DIR)/bin/fromelf --bin $(LINKER_OUTPUT) --output $(OUTPUT_BIN)
	$(ARMCC_ROOT_DIR)/bin/fromelf --i32 $(LINKER_OUTPUT) --output $(OUTPUT_HEX)
endif
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
ifeq ($(findstring y,$(CONFIG_USED_FOR_SEMIHOSTING_UPLOADING)),y)
	$(CP)  $(OUTPUT_BIN) $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
	$(CP) $(OUTPUT_CRC32) $(CONFIG_SEMIHOSTING_UPLOADING_DIR)
endif
