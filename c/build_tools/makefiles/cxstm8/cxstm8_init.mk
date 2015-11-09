#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE


PREPROCESSOR =	"$(CXSTM8_ROOT_DIR)"\cpstm8
CC			=	"$(CXSTM8_ROOT_DIR)\cxstm8"
ASM			=	"$(CXSTM8_ROOT_DIR)\castm8" 


CXSTM8_LIB_ROOT_DIR  = 	$(CXSTM8_ROOT_DIR)/lib


#GLOBAL_CFLAGS := $(GLOBAL_CFLAGS) +modsl0 +split -pp +compact -pc99
GLOBAL_CFLAGS := $(GLOBAL_CFLAGS) +modsl0 +split -pp +compact +debug -pc99
GLOBAL_CFLAGS := $(GLOBAL_CFLAGS) -eg#errors compatible to eclipse


GLOBAL_DEFINES := $(GLOBAL_DEFINES) CXSTM8



endif
### end of section that run just one time per build
#####################################################



