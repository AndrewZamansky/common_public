
##################################################################
#### the following section we need to run just one time per build
#ifndef SKIP_SECTION_THAT_SHOULD_RUN_ONCE_AFTER_AUTO_FILE_GENERATIONS
#

FMT_GLOBAL_INCLUDE_DIR	:= 	$(patsubst %,-I%,$(GLOBAL_INCLUDE_DIR))
FMT_GLOBAL_DEFINES	:= 	$(patsubst %,-D%,$(GLOBAL_DEFINES))
FMT_GLOBAL_ASM_DEFINES := $(patsubst %,--predefine "% SETL {TRUE}",$(GLOBAL_DEFINES))


LDFLAGS = $(GLOBAL_LDFLAGS)

#endif###
### end of section that run just one time per build
#####################################################

define CALCULATE_ALL_INCLUDE_DIRS
	ALL_INCLUDE_DIRS = $(FMT_GLOBAL_INCLUDE_DIR) $(patsubst %,-I%,$(INCLUDE_DIR) $(COMPILER_INCLUDE_DIR))
endef

#substitute " to \" for string defines
define CALCULATE_ALL_DEFINES
	ALL_DEFINES	=  $(subst ",\", $(FMT_GLOBAL_DEFINES) $(patsubst %,-D%,$(DEFINES)) )
endef

define CALCULATE_ALL_ASM_DEFINES
	ALL_ASM_DEFINES	=
	#ALL_ASM_DEFINES	=$(FMT_GLOBAL_ASM_DEFINES)   $(patsubst %,--predefine "% SETL {TRUE}",$(DEFINES))
endef



define CALCULATE_CC_OUTPUT_FLAG_AND_FILE
	CC_OUTPUT_FLAG_AND_FILE = -o $$@#this will transform to $@ in make rule 
endef

define CALCULATE_ASM_OUTPUT_FLAG_AND_FILE
	ASM_OUTPUT_FLAG_AND_FILE = -o $$@#this will transform to $@ in make rule 
endef