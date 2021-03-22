NULL_NAME := $(strip $(NULL_NAME))

ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

internal-all:: $(NULL_NAME:=.all.null.variables);

internal-stage:: $(NULL_NAME:=.stage.null.variables);

NULLS_WITH_SUBPROJECTS = $(strip $(foreach null,$(NULL_NAME),$(patsubst %,$(null),$(call __schema_var_all,$(null)_,SUBPROJECTS))))
ifneq ($(NULLS_WITH_SUBPROJECTS),)
internal-clean:: $(NULLS_WITH_SUBPROJECTS:=.clean.null.subprojects)
endif

$(NULL_NAME):
	$(ECHO_MAKE)$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) $@.all.null.variables

$(eval $(call __mod,master/null.mk))
