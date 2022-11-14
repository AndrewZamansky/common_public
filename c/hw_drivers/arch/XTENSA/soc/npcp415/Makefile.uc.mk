ifeq ($(sort $(CONFIG_NPCP415)),y)
    INCLUDE_THIS_COMPONENT := y

    $(eval $(call ADD_TO_GLOBAL_DEFINES, FUSION_F1_P7))
endif

include $(COMMON_CC)
