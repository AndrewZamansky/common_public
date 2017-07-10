ifneq ($(COMPONENT_PATH),)
    SRC_IDL += $(patsubst %,$(COMPONENT_PATH)/%,$(SRC))
endif
