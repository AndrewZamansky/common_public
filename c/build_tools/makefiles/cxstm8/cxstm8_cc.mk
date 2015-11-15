

FMT_GLOBAL_INCLUDE_DIR	:= 	$(patsubst %,-i%,$(GLOBAL_INCLUDE_DIR))
FMT_GLOBAL_DEFINES	:= 	$(patsubst %,-d%,$(GLOBAL_DEFINES))


define CALCULATE_ALL_INCLUDE_DIRS
	ALL_INCLUDE_DIRS = $(FMT_GLOBAL_INCLUDE_DIR) $(patsubst %,-i%,$(INCLUDE_DIR))
endef

define CALCULATE_ALL_DEFINES
	# do the following : 1 - add '-d' to all local defines , 2 - add global defines , 3 - add '\\\' to string defines
	ALL_DEFINES	=  $(subst ",\\\", $(FMT_GLOBAL_DEFINES) $(patsubst %,-d%,$(DEFINES)) )
endef

define CALCULATE_ALL_ASM_DEFINES
	ALL_ASM_DEFINES	 = 
endef

#define CALCULATE_CC_OUTPUT_FLAG_AND_FILE
#	CC_OUTPUT_FLAG_AND_FILE = -co$(OBJ_DIR)
#endef
define CALCULATE_CC_OUTPUT_FLAG_AND_FILE
	CC_OUTPUT_FLAG_AND_FILE = -co$$(patsubst %/,%,$$(dir $$@))
endef

define CALCULATE_ASM_OUTPUT_FLAG_AND_FILE
	ASM_OUTPUT_FLAG_AND_FILE = -o$$@#this will transform to $@ in make rule 
endef


