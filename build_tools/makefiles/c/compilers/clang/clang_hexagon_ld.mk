#{{{{{{{{  DEFINING OUTPUTS {{{{{{{{

ifdef CONFIG_REDEFINED_OUTPUT_NAME
    CONFIG_REDEFINED_OUTPUT_NAME :=$(patsubst "%",%,$(CONFIG_REDEFINED_OUTPUT_NAME))
    OUTPUT_NAME :=$(CONFIG_REDEFINED_OUTPUT_NAME)
    HISTORY_OUTPUT_NAME :=$(CONFIG_REDEFINED_OUTPUT_NAME)_$(MAIN_VERSION_STR).so
else
    ifdef CONFIG_OUTPUT_TYPE_DYNAMIC_LIBRARY
        OUTPUT_NAME :=lib$(FULL_PROJECT_NAME).so
        HISTORY_OUTPUT_NAME :=lib$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR).so
    else ifdef CONFIG_OUTPUT_TYPE_APPLICATION
        OUTPUT_NAME :=$(FULL_PROJECT_NAME)
        HISTORY_OUTPUT_NAME :=$(FULL_PROJECT_NAME)_$(MAIN_VERSION_STR)
    else
        $(info err: unknown output type)
        $(call exit,1)
    endif
endif

LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_NAME)
LINKER_HISTORY_OUTPUT :=$(OUT_DIR_HISTORY)/$(HISTORY_OUTPUT_NAME)
OUTPUT_ASM :=  $(OUT_DIR)/$(OUTPUT_NAME).asm

LINKER_OUTPUT :=$(call fix_path_if_in_windows,$(LINKER_OUTPUT))
LINKER_HISTORY_OUTPUT :=$(call fix_path_if_in_windows,$(LINKER_HISTORY_OUTPUT))
OUTPUT_CRC32 :=$(call fix_path_if_in_windows,$(OUTPUT_CRC32))

#}}}}}}}}  END OF DEFINING OUTPUTS  }}}}}}}}




#{{{{{{{{   LDFLAGS PREPARATIONS   {{{{{{{{

#init LDFLAGS
LDFLAGS :=

ifdef CONFIG_HEXAGON_VERSION_60
    LDFLAGS += -mv60 
else
    $(info err: unknown hexagon version)
    $(call exit,1)
endif

LDFLAGS += -O0 
LDFLAGS += -mG0lib 
LDFLAGS += -G0 
#LDFLAGS += -v 
LDFLAGS += $(GLOBAL_LDFLAGS)
LDFLAGS += -Wl,-Map=$(OUT_DIR)/$(OUTPUT_NAME).map

LDFLAGS_SO += -fpic 
LDFLAGS_SO += -shared 
LDFLAGS_SO += -Wl,-Bsymbolic
LDFLAGS_SO += -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=free 
LDFLAGS_SO += -Wl,--wrap=realloc -Wl,--wrap=memalign -Wl,--wrap=__stack_chk_fail
LDFLAGS_SO += -Wl,-soname=$(OUTPUT_NAME)


#}}}}}}}}  END OF LDFLAGS PREPARATIONS }}}}}}}}




#{{{{{{{{   LIBRARIES PREPARATIONS {{{{{{{{

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
    STD_LIBRARIES := libc.a
    STD_LIBRARIES +=libm.a libgcc.a libstdc++
    GLOBAL_LIBS += $(STD_LIBRARIES)
endif
GLOBAL_LIBS :=$(sort $(GLOBAL_LIBS)) #remove duplicates
LIBS := $(patsubst lib%,-l%,$(GLOBAL_LIBS))
LIBS := $(patsubst %.a,%,$(LIBS))

GLOBAL_LIBS_PATH :=$(sort $(GLOBAL_LIBS_PATH)) #remove duplicates
LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))

#}}}}}}}}  END OF LIBRARIES PREPARATIONS }}}}}}}}




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



ifdef CONFIG_OUTPUT_TYPE_APPLICATION
    
    LINKER_CMD =$(LD) $(LDFLAGS)  $(LIBRARIES_DIRS) 
    LINKER_CMD += -Wl,--start-group -Wl,--whole-archive
    ifdef CONFIG_HEXAGON_VERSION_60
        LINKER_CMD += $(HEXAGON_ROOT_DIR)/target/hexagon/lib/v60/G0/libhexagon.a
    endif
    LINKER_CMD += $(LIBS) -Wl,--no-whole-archive -Wl,--end-group
    LINKER_CMD += -Wl,--start-group
    LINKER_CMD += -Wl,--dynamic-linker=
    LINKER_CMD += -Wl,-E
    LINKER_CMD += -Wl,--force-dynamic
    LINKER_CMD += $(APQ8096_SRC_DIR)/common/rtld/ship/hexagon_Debug_dynamic/rtld.a
    LINKER_CMD += $(APQ8096_SRC_DIR)/common/a1std/ship/hexagon_Debug_dynamic/a1std.a
    LINKER_CMD += $(APQ8096_SRC_DIR)/audio/voice_imc_utils/ship/hexagon_Debug_dynamic/voice_imc_utils.a
    LINKER_CMD += @$(ALL_OBJECTS_LIST_FILE) -Wl,--end-group -o $(LINKER_OUTPUT)

else ifdef CONFIG_OUTPUT_TYPE_DYNAMIC_LIBRARY 
   
   LINKER_CMD =$(LD) $(LDFLAGS) $(LDFLAGS_SO) $(LIBRARIES_DIRS)
    ifdef CONFIG_HEXAGON_VERSION_60
        LINKER_CMD += -L$(HEXAGON_ROOT_DIR)/target/hexagon/lib/v60/G0/pic
    endif
    LINKER_CMD += @$(ALL_OBJECTS_LIST_FILE) $(LIBS) -o $(LINKER_OUTPUT)
 
endif



ASM_CMD := $(HEXAGON_ROOT_DIR)/bin/hexagon-llvm-objdump.exe
ASM_CMD +=  -disassemble $(LINKER_OUTPUT) > $(OUTPUT_ASM)

build_outputs :
	$(LINKER_CMD)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
	$(ASM_CMD)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
