include $(MAKEFILE_DEFS_ROOT_DIR)/common.mk

#test that arguments are set correctly 
ifeq ($(LIST_FILE_NAME_TRUNCATE),)
    ifeq ($(LIST_FILE_NAME_APPEND),)
        $(info !--- LIST_FILE_NAME_TRUNCATE or LIST_FILE_NAME_APPEND (= name of file to create/append) should be set before including this file)
        $(error )
    endif
endif
ifneq ($(LIST_FILE_NAME_TRUNCATE),)
    ifneq ($(LIST_FILE_NAME_APPEND),)
        $(info !--- only one argument of LIST_FILE_NAME_TRUNCATE or LIST_FILE_NAME_APPEND may be set before including this file)
        $(error )
    endif
endif
ifeq ($(PREFIX_FOR_EACH_ITEM),DUMMY_________123)
    $(info !--- PREFIX_FOR_EACH_ITEM (= string to insert before each item) should be set before including this file)
    $(error )
endif
ifeq ($(ITEMS),DUMMY_________123)
    $(info !--- ITEMS (= items to place in file) should be set before including this file)
    $(error )
endif

ifneq ($(LIST_FILE_NAME_TRUNCATE),)
    LIST_FILE_NAME:=$(LIST_FILE_NAME_TRUNCATE)
    ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
        DUMMY:=$(shell echo|set /p=$(wordlist 1, 1,$(ITEMS))>$(LIST_FILE_NAME))
    else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
        DUMMY:=$(shell echo >$(LIST_FILE_NAME))    
    endif
    ITEMS :=$(wordlist  2,1000000,$(ITEMS))
else    
    LIST_FILE_NAME:=$(LIST_FILE_NAME_APPEND)
endif


_M :=20#too large _M may cause string of _M items be to long for HOST shell(8192 for win) . too small _M can increase build time 
_MPP:=21#must be _M+1
_MAX_N:=1000000
# put_items_to_file is recursive function that receives 2 arguments
# argument $1 contains what to put in shell
# argument $2 contains the remaining list of N items.
# each iteration new $1 will contain first _M items formatted to output to shell
# and new $2 will contain last N-_M items
put_items_to_file=$(shell $1) $(foreach var,$(wordlist 1, 1,$2),\
         $(call put_items_to_file,\
         $(patsubst %, echo|set /p=$(PREFIX_FOR_EACH_ITEM)%>>$(LIST_FILE_NAME) & ,$(wordlist 1,$(_M),$2)),\
         $(wordlist  $(_MPP),$(_MAX_N),$2)))

$(call put_items_to_file,,$(ITEMS))


#clear arguments for next function usage
LIST_FILE_NAME_TRUNCATE:=
LIST_FILE_NAME_APPEND:=
PREFIX_FOR_EACH_ITEM:=DUMMY_________123
ITEMS:=DUMMY_________123
