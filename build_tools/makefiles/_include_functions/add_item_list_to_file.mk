include $(MAKEFILES_ROOT_DIR)/common.mk

#test that arguments are set correctly 
ifeq ($(LIST_FILE_NAME_TRUNCATE),)
    ifeq ($(LIST_FILE_NAME_APPEND),)
        $(info err: LIST_FILE_NAME_TRUNCATE or LIST_FILE_NAME_APPEND {= name 
        $(info    : of file to create/append} should be set before 
        $(info    : including this file)
        $(call exit,1)
    endif
endif
ifneq ($(LIST_FILE_NAME_TRUNCATE),)
    ifneq ($(LIST_FILE_NAME_APPEND),)
        $(info err: only one argument of LIST_FILE_NAME_TRUNCATE or 
        $(info    : LIST_FILE_NAME_APPEND may be set before including this file)
        $(call exit,1)
    endif
endif
ifeq ($(PREFIX_FOR_EACH_ITEM),DUMMY_________123)
    $(info err: PREFIX_FOR_EACH_ITEM {= string to insert before each 
    $(info    : item} should be set before including this file)
    $(call exit,1)
endif
ifeq ($(SUFFIX_LINE_FOR_EACH_ITEM),DUMMY_________123)
    $(info err: SUFFIX_LINE_FOR_EACH_ITEM {= string to insert after each 
    $(info    : item} should be set before including this file)
    $(call exit,1)
endif
ifeq ($(ITEMS),DUMMY_________123)
    $(info err: ITEMS {= items to place in file} should be set before 
    $(info    : including this file)
    $(call exit,1)
endif

ifneq ($(LIST_FILE_NAME_TRUNCATE),)
    LIST_FILE_NAME :=$(call fix_path_if_in_windows,$(LIST_FILE_NAME_TRUNCATE))
    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        DUMMY:=$(shell copy /b /y NUL $(LIST_FILE_NAME))
    else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
        DUMMY:=$(shell cat /dev/null >|$(LIST_FILE_NAME))
    endif
else    
    LIST_FILE_NAME :=$(call fix_path_if_in_windows,$(LIST_FILE_NAME_APPEND))
endif

_ECHO_PREFIX :=echo $(PREFIX_FOR_EACH_ITEM)

_ECHO_SUFFIX :=
ifneq ($(SUFFIX_LINE_FOR_EACH_ITEM),)
    # use ':=' at the end, to calculate variable right here and not each item
    _ECHO_SUFFIX =>>$(LIST_FILE_NAME)$(SHELL_CMD_DELIMITER)
    _ECHO_SUFFIX += echo $(SUFFIX_LINE_FOR_EACH_ITEM)
endif
_ECHO_SUFFIX :=$(_ECHO_SUFFIX)>>$(LIST_FILE_NAME)$(SHELL_CMD_DELIMITER)

WORKSPACE_CFG_MK :=$(PARENT_OF_COMMON_PUBLIC_DIR)/workspace_config.mk
NUM_OF_WORDS_VAR :=\
          $(WORKSPACE_CFG_MK)/REDEFINE_NUM_OF_WORDS_IN_SHELL_COMMAND

add_item_to_file_stop=$(if $1,\
        $(eval $(info err: shell command is too long on Windows systems)\
               $(info ---: most common reason: pathes are too long)\
               $(info ---: possible solutions:)\
               $(info ---: 1. move workspace to shorter path.)\
               $(info ---: 2. set $(NUM_OF_WORDS_VAR) to some low number.\
                              (default is 16).)\
               $(info ---:    too low value will cause slower execution.)\
               $(call exit,1)),\
        do nothing)

# step2: test cmd length and if it's short enough then run shell
add_item_to_file_step2 =$(eval _is_long=$(call check_win_cmd_len,$1))\
          $(eval $(if $(_is_long), $(call add_item_to_file_stop,1),\
          $(shell $1)))

# step1: create shell command string
add_item_to_file_step1 =$(call\
    add_item_to_file_step2, $(patsubst %, $(_ECHO_PREFIX)%$(_ECHO_SUFFIX),$1))

# add_one function will take _nums[$(1)] value, that is actually ($(1) + 1)
_nums = 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26\
          27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48
add_one =$(if $1,$(eval _tmp =$(word $1,$(_nums)))$(if $(_tmp),$(_tmp),\
                   $(info err: _M value should be less then 48)$(call exit,1)),)

# too large _M may cause string of _M items be to long for HOST
# shell(8192 for win). too small _M can increase build time
_M := $(if $(REDEFINE_NUM_OF_WORDS_IN_SHELL_COMMAND),\
           $(REDEFINE_NUM_OF_WORDS_IN_SHELL_COMMAND), 16)
_MPP:=$(call add_one,$(_M))#must be _M+1
_MAX_N:=1000000
# argument $1 contains list of items to put in shell
# each iteration call to 'add_item_to_file' contain first _M items
# that will be formatted and outputed to file.
# in recursive call to 'put_items_to_file' argument $1 will
# contains the remaining list of N-$(_M) items.
# 'if' statement will continue to call 'put_items_to_file' till $1 is empty
put_items_to_file= $(if $1,\
      $(call add_item_to_file_step1,$(wordlist 1,$(_M),$1)) \
      $(call put_items_to_file,$(strip $(wordlist $(_MPP),$(_MAX_N),$1))),\
      DUMMY_DO_NOTHING_ON_IF_FALSE_CONDITION)

DUMMY := $(call put_items_to_file,$(ITEMS))


#clear arguments for next function usage
LIST_FILE_NAME_TRUNCATE:=
LIST_FILE_NAME_APPEND:=
PREFIX_FOR_EACH_ITEM:=DUMMY_________123
SUFFIX_LINE_FOR_EACH_ITEM:=DUMMY_________123
ITEMS:=DUMMY_________123
