SUBPROJECT_NAME := $(strip $(SUBPROJECT_NAME))

ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

internal-all:: $(SUBPROJECT_NAME:=.all.subproject.variables);

internal-stage:: $(SUBPROJECT_NAME:=.stage.subproject.variables);

SUBPROJECTS_WITH_SUBPROJECTS = $(strip $(foreach subproject,$(SUBPROJECT_NAME),$(patsubst %,$(subproject),$(call __schema_var_all,$(subproject)_,SUBPROJECTS))))
ifneq ($(SUBPROJECTS_WITH_SUBPROJECTS),)
internal-clean:: $(SUBPROJECTS_WITH_SUBPROJECTS:=.clean.subproject.subprojects)
endif

$(SUBPROJECT_NAME):
	$(ECHO_MAKE)$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) $@.all.subproject.variables

$(eval $(call __mod,master/subproject.mk))
