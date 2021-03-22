SIMBLTWEAK_NAME := $(strip $(SIMBLTWEAK_NAME))

ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

internal-all:: $(SIMBLTWEAK_NAME:=.all.simbltweak.variables);

internal-stage:: $(SIMBLTWEAK_NAME:=.stage.simbltweak.variables);

internal-after-install::

SIMBLTWEAKS_WITH_SUBPROJECTS = $(strip $(foreach simbltweak,$(SIMBLTWEAK_NAME),$(patsubst %,$(simbltweak),$(call __schema_var_all,$(simbltweak)_,SUBPROJECTS))))
ifneq ($(SIMBLTWEAKS_WITH_SUBPROJECTS),)
internal-clean:: $(SIMBLTWEAKS_WITH_SUBPROJECTS:=.clean.simbltweak.subprojects)
endif

$(SIMBLTWEAK_NAME):
	$(ECHO_MAKE)$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) $@.all.simbltweak.variables

$(eval $(call __mod,master/simbltweak.mk))
