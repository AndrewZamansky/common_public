
#{{{{{{{{   LDFLAGS PREPARATIONS   {{{{{{{{

#init LDFLAGS
LDFLAGS :=

ifdef CONFIG_HEXAGON_VERSION_60
    LDFLAGS += -mv60 
else
    $(error unknown hexagon version)
endif

LDFLAGS += -O0 
LDFLAGS += -mG0lib 
LDFLAGS += -G0 
#LDFLAGS += -v 
LDFLAGS += $(GLOBAL_LDFLAGS)
LDFLAGS += -Wl,-Map=$(OUT_DIR)/$(OUTPUT_APP_NAME).map

LDFLAGS_SO += -fpic 
LDFLAGS_SO += -shared 
LDFLAGS_SO += -Wl,-Bsymbolic
LDFLAGS_SO += -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=free 
LDFLAGS_SO += -Wl,--wrap=realloc -Wl,--wrap=memalign -Wl,--wrap=__stack_chk_fail
LDFLAGS_SO += -Wl,-soname=$(OUTPUT_APP_NAME).so
LDFLAGS_SO += -Wl,-Map=$(OUT_DIR)/$(OUTPUT_APP_NAME).so.map


#}}}}}}}}  END OF LDFLAGS PREPARATIONS }}}}}}}}




#{{{{{{{{   LIBRARIES PREPARATIONS {{{{{{{{

ifdef CONFIG_INCLUDE_TOOLCHAIN_LIBRARIES
    STD_LIBRARIES := libc.a
    STD_LIBRARIES +=libm.a libgcc.a libstdc++
    GLOBAL_LIBS += $(STD_LIBRARIES)
endif
LIBS := $(patsubst lib%,-l%,$(GLOBAL_LIBS))
LIBS := $(patsubst %.a,%,$(LIBS))

LIBRARIES_DIRS := $(patsubst %,-L%,$(GLOBAL_LIBS_PATH))

#}}}}}}}}  END OF LIBRARIES PREPARATIONS }}}}}}}}




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
include $(MAKEFILES_INC_FUNC_DIR)/add_item_list_to_file_in_one_line.mk
#end of file creation

#}}}}}}}}  END OF CREATING OBJECT LIST  }}}}}}}}



#{{{{{{{{  DEFINING OUTPUTS {{{{{{{{

LINKER_OUTPUT := $(OUT_DIR)/$(OUTPUT_APP_NAME)
HISTORY_OUT_PREFIX :=$(OUT_DIR_HISTORY)/$(PROJECT_NAME)_$(MAIN_VERSION_STR)
ifdef CONFIG_CLANG_OUTPUT_TYPE_DYNAMIC_LIBRARY
    LINKER_OUTPUT :=$(LINKER_OUTPUT).so
    HISTORY_OUT_PREFIX :=$(HISTORY_OUT_PREFIX).so
else ifdef CONFIG_CLANG_OUTPUT_TYPE_APPLICATION
    #do nothing for now
else
    $(error ---- unknown output type ----)
endif
#OUTPUT_ASM :=  $(OUT_DIR)/$(OUTPUT_APP_NAME).asm


ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    LINKER_OUTPUT := $(subst /,\,$(LINKER_OUTPUT))
    LINKER_HISTORY_OUTPUT := $(subst /,\,$(LINKER_HISTORY_OUTPUT))
    OUTPUT_CRC32 := $(subst /,\,$(OUTPUT_CRC32))
endif

ifeq ($(findstring y,$(CONFIG_USED_FOR_SEMIHOSTING_UPLOADING)),y)
    CONFIG_CALCULATE_CRC32=y
endif

#}}}}}}}}  END OF DEFINING OUTPUTS  }}}}}}}}


ifdef CONFIG_CLANG_OUTPUT_TYPE_APPLICATION
    
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

else ifdef CONFIG_CLANG_OUTPUT_TYPE_DYNAMIC_LIBRARY 
   
   LINKER_CMD =$(LD) $(LDFLAGS) $(LDFLAGS_SO) $(LIBRARIES_DIRS)
    ifdef CONFIG_HEXAGON_VERSION_60
        LINKER_CMD += -L$(HEXAGON_ROOT_DIR)/target/hexagon/lib/v60/G0/pic
    endif
    LINKER_CMD += @$(ALL_OBJECTS_LIST_FILE) $(LIBS) -o $(LINKER_OUTPUT)
 
endif



#	$(FULL_GCC_PREFIX)objdump -d -S $(LINKER_OUTPUT) > $(OUTPUT_ASM)

build_outputs :
	$(LINKER_CMD)
	$(CP)  $(LINKER_OUTPUT) $(LINKER_HISTORY_OUTPUT)
ifeq ($(findstring y,$(CONFIG_CALCULATE_CRC32)),y)
	$(CRC32CALC) $(OUTPUT_BIN) > $(OUTPUT_CRC32)
endif
	