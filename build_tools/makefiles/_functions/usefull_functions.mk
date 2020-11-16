current_dir :=$(dir $(abspath $(firstword $(MAKEFILE_LIST))))
abspath_fix =$(patsubst $(current_dir)%,%,$(abspath $(1) ))

calc_parent_dir = $(patsubst  %/,%, $(dir $(abspath $(1) )))

ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    ifeq ($(findstring 3.81,$(MAKE_VERSION)),3.81)
        calc_parent_dir = $(patsubst  %/,%, $(dir $(call abspath_fix,$(1) )))
    endif
endif

# function : mkdir_if_not_exists
# usage    : $(call mkdir_if_not_exists, /path/to/dir)
#
# added current directory (.) to wildcard because during function
# initialization the $(1) is empty . so else-part calculated like $(shell mkdir)
# and this produce error (at least in Windows)
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
define mkdir_if_not_exists
    $(if $(wildcard $(firstword $(1) .)),,$(shell $(MKDIR) $(subst /,\,$(1))))
endef
else
define mkdir_if_not_exists
    $(if $(wildcard $(firstword $(1) .)),,$(shell $(MKDIR) $(1)))
endef
endif

# function : exit
# usage    : $(call exit,1)
#
# shell access is used to delay execution to allow all previous printings
# to be outputed
define exit
    $(if $(1),$(shell echo dummy ) $(error stopping ...),)
endef


# function : check_if_selected
# usage    : $(call check_if_selected,$(VAR))
#
# return 'y' if all of words in $(VAR)
# are 'y'. returns empty string in other case
define eval_config
$(if $(strip $(filter-out y,$(1))),,$(filter y,$(1)))
endef


# function : fix_path_if_in_windows
# usage    : $(call fix_path_if_in_windows,$(VAR))
#
# changes slash '/' to backslash '\' if we are building in Windows OS
fix_path_if_in_windows =$(if \
     $(findstring WINDOWS,$(COMPILER_HOST_OS)),$(subst /,\,$1),$1)





# function: check_win_cmd_len
# usage    : $(call check_win_cmd_len,$(CMD_VAR))
#
# $1-$(CMD_VAR): cmd string
# if we are building in Windows7 and length of tested
# string is more then 8196 then build proccess will stop because
# of windows shell limitiation on command length.
# so we take lower limit: 8100.

# functions for string length :
ASCII := A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
ASCII += a b c d e f g h i j k l m n o p q r s t u v w x y z
ASCII += 0 1 2 3 4 5 6 7 8 9
ASCII += ` ~ ! @ \# $ % ^ & * ( ) - _ = +
ASCII += { } [ ] \ : ; ' " < > , . / ? |

SPACE:=
SPACE:=$(SPACE) $(SPACE)

# eval will output empty string (only all_chars will be updated), so
# the final string will be $(all_chars)
separate_chars_step2 =$(strip $(eval all_chars :=$1)\
    $(foreach a,$(ASCII),$(eval all_chars := $$(subst $$a,$$a ,$(all_chars))))\
    $(all_chars))

# step1: replace spaces by '_' so spaces will also be counted
separate_chars_step1 =$(call separate_chars_step2,$(subst $(SPACE),_,$1))

separate_chars =$(call separate_chars_step1,$1)

strlen =$(words $(call separate_chars,$1))



# function : count_extra_chars
# count_extra_chars will return 0 if length of string less then $2,
# in other case (strlen - ($2 - 1)) is returned

# step4: count chars left after step3. if nothing left return empty string
count_extra_chars_step4 =$(words $1)


# step3: remove all chars on position $2 and higher
# $(if ) is used to avoid error during makefile parsing when $2 is empty
count_extra_chars_step3 =$(if $2,$(call \
                  count_extra_chars_step4,$(wordlist $2,100000000,$1)),)

# step2: replace $1 by string with chars separated by spaces
count_extra_chars_step2 =$(call \
                  count_extra_chars_step3,$(call separate_chars,$1),$2)

# step1: replace '$' by '_'. otherwise expression is expended when not needed
count_extra_chars_step1 =$(call count_extra_chars_step2,$(subst $$,_,$1),$2)

count_extra_chars =$(call count_extra_chars_step1,$1,$2)

# function: test_if_string_len_less_or_eq
# $1: string
# $2: tested length
# if length of tested string less then $2 then count_extra_chars will
# returns '0'. so $(patsubst ) will create empty string, so 'yes' returned
test_if_string_len_less =$(if \
         $(patsubst 0,,$(call count_extra_chars,$1,$2)),no,yes)


__check_cmd_len =$(if \
    $(findstring no,$(call test_if_string_len_less,$1,8100)),TOO_LONG,)

check_win_cmd_len =$(if \
      $(findstring WINDOWS,$(COMPILER_HOST_OS)),$(call __check_cmd_len,$1),)
####################################################################






# function: reduce_cmd_len
# usage    : $(call reduce_cmd_len,$(CMD_VAR))
#
# $1-$(CMD_VAR): cmd line string
# function will add environmantal variables to command to replace
# some pathes
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    ENV_VAR_PUBLIC_SW_PACKAGES_DIR :=%PUBLIC_SW_PACKAGES_DIR%
    ENV_VAR_EXTERNAL_SOURCE_ROOT_DIR :=%EXTERNAL_SOURCE_ROOT_DIR%
    ENV_VAR_PUBLIC_DRIVERS_DIR :=%PUBLIC_DRIVERS_DIR%
    ENV_VAR_COMMON_PRIVATE_DIR :=%COMMON_PRIVATE_DIR%
    ENV_VAR_APP_ROOT_DIR :=%APP_ROOT_DIR%
else
    ENV_VAR_PUBLIC_SW_PACKAGES_DIR :=$${PUBLIC_SW_PACKAGES_DIR}
    ENV_VAR_EXTERNAL_SOURCE_ROOT_DIR :=$${EXTERNAL_SOURCE_ROOT_DIR}
    ENV_VAR_PUBLIC_DRIVERS_DIR :=$${PUBLIC_DRIVERS_DIR}
    ENV_VAR_COMMON_PRIVATE_DIR :=$${COMMON_PRIVATE_DIR}
    ENV_VAR_APP_ROOT_DIR :=$${APP_ROOT_DIR}
endif

reduce_cmd_len_1 =$(subst\
       $(PUBLIC_SW_PACKAGES_DIR),$(ENV_VAR_PUBLIC_SW_PACKAGES_DIR),$1)
reduce_cmd_len_2 =$(subst\
       $(EXTERNAL_SOURCE_ROOT_DIR),$(ENV_VAR_EXTERNAL_SOURCE_ROOT_DIR),\
       $(call reduce_cmd_len_1,$1))
reduce_cmd_len_3 =$(subst\
       $(PUBLIC_DRIVERS_DIR),$(ENV_VAR_PUBLIC_DRIVERS_DIR),\
       $(call reduce_cmd_len_2,$1))
reduce_cmd_len_4 =$(subst\
       $(COMMON_PRIVATE_DIR),$(ENV_VAR_COMMON_PRIVATE_DIR),\
       $(call reduce_cmd_len_3,$1))
# for windows: replace also string with backslashs :
reduce_cmd_len_5 =$(subst\
       $(subst \,/,$(COMMON_PRIVATE_DIR)),$(ENV_VAR_COMMON_PRIVATE_DIR),\
       $(call reduce_cmd_len_4,$1))
reduce_cmd_len_6 =$(subst\
        $(APP_ROOT_DIR),$(ENV_VAR_APP_ROOT_DIR),$(call reduce_cmd_len_5,$1))
# for windows: replace also string with backslashs :
reduce_cmd_len_7 =$(subst\
       $(subst /,\,$(APP_ROOT_DIR)),$(ENV_VAR_APP_ROOT_DIR),\
       $(call reduce_cmd_len_6,$1))

reduce_cmd_len =$(SET_CC_ENV_VARS) $(call reduce_cmd_len_7,$1)
########################################################################





# function: fwrite
# usage    : DUMMY := $(call fwrite,$(FILE_NAME),$(FILE_DATA),$(OPTIONS))
#
# $1-$(FILE_NAME): file name
# $2-$(DATA): file data to write
# $3-$(OPTIONS): {TRUNCATE, APPEND}
# function will write data to file
ifeq ($(findstring WINDOWS,$(COMPILER_HOST_OS)),WINDOWS)
    _fwrite_create_file_os = $(if $1,\
          $(shell copy /y /b NUL $(call fix_path_if_in_windows,$2)),do nothing)

    _fwrite_add_item_to_file_stop=$(if $1,\
        $(eval $(info err: shell command is too long on Windows systems)\
               $(info ---: most common reason: pathes are too long)\
               $(info ---: possible solutions:)\
               $(info ---: 1. move workspace to shorter path.)\
               $(info ---: 2. set $(NUM_OF_WORDS_VAR) to some low number.\
                              (default is 20).)\
               $(info ---:    too low value will cause slower execution.)\
               $(call exit,1)),\
          do nothing)

    _fwrite_add_item_to_file_step2=$(if $1,\
          $(eval _is_long=$(call check_win_cmd_len,$1))\
          $(eval $(if $(_is_long), $(call 1,_fwrite_add_item_to_file_stop),\
          $(shell $1))),\
          do nothing)
    _fwrite_add_item_to_file_step1 = $(if $1,\
          $(call _fwrite_add_item_to_file_step2,echo|set /p dummy="$3 ">>$2),\
          do nothing)
    _fwrite_add_item_to_file = $(if $1,\
          $(call _fwrite_add_item_to_file_step1,1,$2,$3),\
          do nothing)
else ifeq ($(findstring LINUX,$(COMPILER_HOST_OS)),LINUX)
    #_ECHO_PREFIX :=/bin/echo -n #last space is important
    _fwrite_create_file_os = $(if $1,\
          $(shell /bin/echo -n >$(2)),do nothing)
    _fwrite_add_item_to_file = $(if $1,\
          $(shell /bin/echo -n $3\ >>$2),do nothing)
endif

_wrong_frite_option =$(if $1,\
                       $(info err: only TRUNCATE or APPEND options are valid)\
                       $(info ---: options for fwrite)\
                       $(call exit,1),\
                       do nothing)

_check_options =$(if $2,\
                  $(if $(filter-out TRUNCATE APPEND,$2),\
                    $(call _wrong_frite_option,1),\
                    valid options),\
                  do nothing)

_fwrite_create_file =\
     $(if $(1),\
       $(eval DUMMY=$(call _check_options,$2,$3))\
       $(if $(filter TRUNCATE,$(3)),\
         $(call _fwrite_create_file_os,1,$(2)),\
         don't create new file)\
       do nothing)

# add_one function will take _nums[$(1)] value, that is actually ($(1) + 1)
_nums = 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26\
          27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48
add_one =$(if $1,$(eval _tmp =$(word $1,$(_nums)))$(if $(_tmp),$(_tmp),\
                   $(info err: _M value should be less then 48)$(call exit,1)),)

# too large _M may cause string of _M items be to long for HOST
# shell(8192 for win). too small _M can increase build time
_M := $(if $(REDEFINE_NUM_OF_WORDS_IN_SHELL_COMMAND),\
           $(REDEFINE_NUM_OF_WORDS_IN_SHELL_COMMAND), 19)
_MPP:=$(call add_one,$(_M))#must be _M+1
_MAX_N:=1000000
# argument $1 contains list of items to put in shell
# each iteration call to 'add_item_to_file' contain first _M items
# that will be formatted and outputed to file.
# in recursive call to 'put_items_to_file' argument $1 will
# contains the remaining list of N-$(_M) items.
# 'if' statement will continue to call 'put_items_to_file' till $1 is empty
_fwrite_put_to_file = $(if $2,\
   $(call _fwrite_add_item_to_file,1,$(1),$(wordlist 1,$(_M),$(2))) \
   $(call _fwrite_put_to_file,$1,$(strip $(wordlist $(_MPP),$(_MAX_N),$2))),\
   do nothing)

_fwrite =$(if $(1),\
    $(call _fwrite_create_file,1,$2,$4) $(call _fwrite_put_to_file,$(2),$(3)),\
    do nothing)

fwrite =$(if $(1),$(call _fwrite,1,$(1),$(2),$(3)),do nothing)
#DUMMY := $(call fwrite,/home/andrew/Work/tmp/a.txt,a b c a b c a b c a b c)
