#################################################################
### the following section we need to run just one time per build
ifndef COMMON_INIT_SECTION_THAT_SHOULD_RUN_ONCE

ifdef REDEFINE_CXSTM8_ROOT_DIR
    $(info  cxstm8 dir  redefined to $(REDEFINE_CXSTM8_ROOT_DIR) )
    ifeq ("$(wildcard $(REDEFINE_CXSTM8_ROOT_DIR))","")
        $(info !--- cxstm8 path $(REDEFINE_CXSTM8_ROOT_DIR) dont exists)
        $(info !--- to use default cxstm8 location remove/comment REDEFINE_CXSTM8_ROOT_DIR variable in  $(WORKSPACE_ROOT_DIR)/workspace_config.mk )
        $(info !--- you can set customized cxstm8 path in REDEFINE_CXSTM8_ROOT_DIR variable in $(WORKSPACE_ROOT_DIR)/workspace_config.mk )
        $(error )
    endif
    CXSTM8_ROOT_DIR 	:= 	$(REDEFINE_ARM_GCC_ROOT_DIR)
else
    $(info  looking for cxstm8 in default location)
    TEST_CXSTM8_ROOT_DIR 	:=C:\Program\ Files\ (x86)\COSMIC\CXSTM8_EVAL
#    TEST_CXSTM8_ROOT_DIR 	:=C:\Program\ Files\ (x86)\COSMIC\FSE_Compilers\CXSTM8
    ifeq ("$(wildcard $(TEST_CXSTM8_ROOT_DIR))","")
        $(info !--- cxstm8 path $(TEST_CXSTM8_ROOT_DIR) dont exists )
        $(info !--- download cxstm8 and install it to C:\Program Files (x86)\COSMIC\FSE_Compilers  )
        $(info !--- make sure that Hstm8  folder is located in $(TEST_CXSTM8_ROOT_DIR)/  after installing   )
        $(error )
    endif
 #  CXSTM8_ROOT_DIR 	:=C:\Program Files (x86)\COSMIC\FSE_Compilers\CXSTM8
   CXSTM8_ROOT_DIR 	:=C:\Program Files (x86)\COSMIC\CXSTM8_EVAL
endif



# replce "\ " with " "
EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)
CXSTM8_ROOT_DIR :=$(subst \$(SPACE),$(SPACE),$(CXSTM8_ROOT_DIR))


PREPROCESSOR =	"$(CXSTM8_ROOT_DIR)\cpstm8"
CC			=	"$(CXSTM8_ROOT_DIR)\cxstm8"
ASM			=	"$(CXSTM8_ROOT_DIR)\castm8" 


CXSTM8_LIB_ROOT_DIR  = 	$(CXSTM8_ROOT_DIR)/lib


#GLOBAL_CFLAGS := $(GLOBAL_CFLAGS) +modsl0 +split -pp +compact -pc99
GLOBAL_CFLAGS := $(GLOBAL_CFLAGS) +modsl0 +split -pp +compact +debug -pc99
GLOBAL_CFLAGS := $(GLOBAL_CFLAGS) -eg#errors compatible to eclipse


endif
### end of section that run just one time per build
#####################################################



