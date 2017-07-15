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
RM        :=rmdir /S /Q
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
