TWEAK_NAME := $(strip $(TWEAK_NAME))

ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

internal-all:: $(TWEAK_NAME:=.all.tweak.variables);

internal-stage:: $(TWEAK_NAME:=.stage.tweak.variables);

ifneq ($(TWEAK_TARGET_PROCESS),)
INSTALL_TARGET_PROCESSES += $(TWEAK_TARGET_PROCESS)
endif

TWEAKS_WITH_SUBPROJECTS = $(strip $(foreach tweak,$(TWEAK_NAME),$(patsubst %,$(tweak),$(call __schema_var_all,$(tweak)_,SUBPROJECTS))))
ifneq ($(TWEAKS_WITH_SUBPROJECTS),)
internal-clean:: $(TWEAKS_WITH_SUBPROJECTS:=.clean.tweak.subprojects)
endif

$(TWEAK_NAME):
	$(ECHO_MAKE)$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) $@.all.tweak.variables

$(eval $(call __mod,master/tweak.mk))
