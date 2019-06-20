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
ifeq ($(ITEMS),DUMMY_________123)
    $(info err: ITEMS {= items to place in file} should be set before including 
    $(info    : this file)
    $(call exit,1)
endif

ifneq ($(LIST_FILE_NAME_TRUNCATE),)
    LIST_FILE_NAME :=$(call fix_path_if_in_windows,$(LIST_FILE_NAME_TRUNCATE))
    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        _ECHO_PREFIX :=echo|set /p=$(PREFIX_FOR_EACH_ITEM)
        DUMMY:=$(shell echo|set /p=$(wordlist 1, 1,$(ITEMS))>$(LIST_FILE_NAME))
        _ECHO_SUFFIX :=>>$(LIST_FILE_NAME) &
    else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
        _ECHO_PREFIX :=/bin/echo -n #last space is important
        DUMMY:=$(shell echo -n $(wordlist 1, 1,$(ITEMS))\ >$(LIST_FILE_NAME))
         _ECHO_SUFFIX :=\ >>$(LIST_FILE_NAME);
     endif
    ITEMS :=$(wordlist  2,1000000,$(ITEMS))
else    
    LIST_FILE_NAME :=$(call fix_path_if_in_windows,$(LIST_FILE_NAME_APPEND))
endif

add_item_to_file=$(shell $(patsubst %, $(_ECHO_PREFIX)%$(_ECHO_SUFFIX),$1))

# too large _M may cause string of _M items be to long for HOST 
# shell(8192 for win). too small _M can increase build time
_M :=20 
_MPP:=21#must be _M+1
_MAX_N:=1000000
# argument $1 contains list of items to put in shell
# each iteration call to 'add_item_to_file' contain first _M items
# that will be formatted and outputed to file.
# in recursive call to 'put_items_to_file' argument $1 will
# contains the remaining list of N-$(_M) items.
# 'if' statement will continue to call 'put_items_to_file' till $1 is empty
put_items_to_file= $(if $1,\
      $(call add_item_to_file,$(wordlist 1,$(_M),$1)) \
      $(call put_items_to_file,$(strip $(wordlist $(_MPP),$(_MAX_N),$1))),\
      DUMMY_DO_NOTHING_ON_IF_FALSE_CONDITION)

DUMMY := $(call put_items_to_file,$(ITEMS))


#clear arguments for next function usage
LIST_FILE_NAME_TRUNCATE:=
LIST_FILE_NAME_APPEND:=
PREFIX_FOR_EACH_ITEM:=DUMMY_________123
ITEMS:=DUMMY_________123
