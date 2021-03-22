APPLICATION_NAME := $(strip $(APPLICATION_NAME))

ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

internal-all:: $(APPLICATION_NAME:=.all.application.variables);

internal-stage:: $(APPLICATION_NAME:=.stage.application.variables);

# Maybe, disabled for further discussion
# install.exec "uicache"
internal-after-install::

APPLICATIONS_WITH_SUBPROJECTS = $(strip $(foreach application,$(APPLICATION_NAME),$(patsubst %,$(application),$(call __schema_var_all,$(application)_,SUBPROJECTS))))
ifneq ($(APPLICATIONS_WITH_SUBPROJECTS),)
internal-clean:: $(APPLICATIONS_WITH_SUBPROJECTS:=.clean.application.subprojects)
endif

$(APPLICATION_NAME):
	$(ECHO_MAKE)$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) $@.all.application.variables

$(eval $(call __mod,master/application.mk))
