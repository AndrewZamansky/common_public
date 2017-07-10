include $(MAKEFILES_ROOT_DIR)/common.mk

OUTPUT_CRC32 :=  $(OUT_DIR)/$(OUTPUT_APP_NAME).crc32


QAIC_ROOT_DIR :=

####### test for existence of qaic and put its directory name in QAIC_ROOT_DIR #####
SEARCHED_TOOL:=qaic
SEARCHED_DIR_VARIABLE:=QAIC_ROOT_DIR
MANUALLY_DEFINED_DIR_VARIABLE:=REDEFINE_QAIC_ROOT_DIR
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    TEST_FILE_IN_SEARCHED_DIR:=qaic.exe
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    TEST_FILE_IN_SEARCHED_DIR:=qaic
endif
include $(MAKEFILES_ROOT_DIR)/_include_functions/tool_existence_check.mk
####### end of tool existence test #####

FMT_GLOBAL_INCLUDE_DIR	:= 	$(patsubst %,-I%,$(GLOBAL_INCLUDE_DIR))
ALL_INCLUDE_DIRS = $(FMT_GLOBAL_INCLUDE_DIR)

QAIC :=$(QAIC_ROOT_DIR)/qaic

build_outputs :
	$(QAIC)  -mdll $(ALL_INCLUDE_DIRS) -o $(OUT_DIR) $(SRC_IDL)
