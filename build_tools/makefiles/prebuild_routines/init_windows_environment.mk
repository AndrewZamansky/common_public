SPACE:= $(EMPTY) $(EMPTY)
MKDIR=mkdir    

CONFIG_SEMIHOSTING_UPLOADING_DIR :=c:\Temp

#    CRC32CALC    =    $(TOOLS_ROOT_DIR)\crc32\crc32.exe

#if $(CONFIG_SEMIHOSTING_UPLOADING_DIR) dont exists then create it
ifeq ($(wildcard $(CONFIG_SEMIHOSTING_UPLOADING_DIR)),) 
   DUMMY:=$(shell $(MKDIR)  $(CONFIG_SEMIHOSTING_UPLOADING_DIR))
endif

COMMON_PARTITION := $(firstword $(subst :, ,$(BUILD_TOOLS_ROOT_DIR))):
APP_PARTITION := $(firstword $(subst :, ,$(APP_ROOT_DIR))):
RMDIR     :=rmdir /S /Q
RM        :=del /F /Q
CP        :=copy /Y

SHELL_OUTPUT :=$(shell WMIC Path Win32_LocalTime Get Year /value)
YEAR := $(word 2,$(subst =,$(SPACE),$(strip $(SHELL_OUTPUT))))
SHELL_OUTPUT :=$(shell WMIC Path Win32_LocalTime Get Month /value)
MONTH := $(word 2,$(subst =,$(SPACE),$(strip $(SHELL_OUTPUT))))
ifeq (1,$(MONTH))
    MONTH :=01
else
    ifneq ($(patsubst 1%,X,$(MONTH)),X)
        MONTH :=0$(MONTH)
    endif
endif

SHELL_OUTPUT :=$(shell WMIC Path Win32_LocalTime Get DAY /value)
DAY := $(word 2,$(subst =,$(SPACE),$(strip $(SHELL_OUTPUT))))
ifeq (1,$(DAY))
    DAY :=01
else ifeq (2,$(DAY))
    DAY :=02
else ifeq (3,$(DAY))
    DAY :=03
else 
    ifneq ($(patsubst 1%,X,$(DAY)),X)
        ifneq ($(patsubst 2%,X,$(DAY)),X)
            ifneq ($(patsubst 3%,X,$(DAY)),X)
                DAY :=0$(DAY)
            endif
        endif
    endif
endif

DATE    :=$(YEAR)-$(MONTH)-$(DAY)

SHELL_OUTPUT :=$(shell WMIC Path Win32_LocalTime Get Hour /value)
HOUR := $(word 2,$(subst =,$(SPACE),$(strip $(SHELL_OUTPUT))))
SHELL_OUTPUT :=$(shell WMIC Path Win32_LocalTime Get Minute /value)
Minute := $(word 2,$(subst =,$(SPACE),$(strip $(SHELL_OUTPUT))))
TIME    :=$(HOUR):$(Minute)
SHELL_CMD_DELIMITER = &
CD :=  $(COMMON_PARTITION) & cd


# calculate maximum length of command line in windows
VAR_LEN_100 :=123456789 123456789 123456789 123456789\
      123456789 123456789 123456789 123456789 123456789 123456789
VAR_LEN_1000 :=!VAR!!VAR!!VAR!!VAR!!VAR!!VAR!!VAR!!VAR!!VAR!!VAR!
CMD :=cmd.exe /v /c "set VAR=$(VAR_LEN_100) &\
      set VAR_LEN_1000=$(VAR_LEN_1000)& echo !VAR_LEN_1000!

SIZES :=0 10 1000 2000 3000 4000 5000 6000 7000 8000 9000 10000 11000
# try to echo 1000 symbols, if output is not empty, remove first word from
# $SIZES, increase the 'echo' by additional 1000 chars and try again. And so on.
# If output is empty, return $(SIZE)[0] - it will be the previous succesfull
# execution of echo command
calc_max_cmd_len = \
   $(if $(and $1,$(SIZES)),\
        $(eval SHELL_OUTPUT :=$(shell $1))\
        $(if $(SHELL_OUTPUT),\
             $(eval SIZES :=$(wordlist 2, 1000, $(SIZES)))\
              $(call calc_max_cmd_len, $(1)!VAR_LEN_1000!),\
             $(word 1, $(SIZES))), \
    )# return empy if $1 and $(SIZES) is empty for first 'if'
SHELL_OUTPUT :=
MAX_WIN_CMD_LEN :=$(strip $(call calc_max_cmd_len,$(CMD)))

# set default to 3000
MAX_WIN_CMD_LEN :=$(if $(MAX_WIN_CMD_LEN),$(MAX_WIN_CMD_LEN),3000)
