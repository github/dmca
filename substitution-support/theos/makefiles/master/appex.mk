APPEX_NAME := $(strip $(APPEX_NAME))

ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

internal-all:: $(APPEX_NAME:=.all.appex.variables);

internal-stage:: $(APPEX_NAME:=.stage.appex.variables);

APPEXS_WITH_SUBPROJECTS = $(strip $(foreach appex,$(APPEX_NAME),$(patsubst %,$(appex),$(call __schema_var_all,$(appex)_,SUBPROJECTS))))
ifneq ($(APPEXS_WITH_SUBPROJECTS),)
internal-clean:: $(APPEXS_WITH_SUBPROJECTS:=.clean.appex.subprojects)
endif

$(APPEX_NAME):
	$(ECHO_MAKE)$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) $@.all.appex.variables

$(eval $(call __mod,master/appex.mk))
