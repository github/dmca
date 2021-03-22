FRAMEWORK_NAME := $(strip $(FRAMEWORK_NAME))

ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

internal-all:: $(FRAMEWORK_NAME:=.all.framework.variables);

internal-stage:: $(FRAMEWORK_NAME:=.stage.framework.variables);

FRAMEWORKS_WITH_SUBPROJECTS = $(strip $(foreach framework,$(FRAMEWORK_NAME),$(patsubst %,$(framework),$(call __schema_var_all,$(framework)_,SUBPROJECTS))))
ifneq ($(FRAMEWORKS_WITH_SUBPROJECTS),)
internal-clean:: $(FRAMEWORKS_WITH_SUBPROJECTS:=.clean.framework.subprojects)
endif

$(FRAMEWORK_NAME):
	$(ECHO_MAKE)$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) $@.all.framework.variables

$(eval $(call __mod,master/framework.mk))
