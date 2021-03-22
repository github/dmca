LIBRARY_NAME := $(strip $(LIBRARY_NAME))

ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

internal-all:: $(LIBRARY_NAME:=.all.library.variables);

internal-stage:: $(LIBRARY_NAME:=.stage.library.variables);

LIBRARYS_WITH_SUBPROJECTS = $(strip $(foreach library,$(LIBRARY_NAME),$(patsubst %,$(library),$(call __schema_var_all,$(library)_,SUBPROJECTS))))
ifneq ($(LIBRARYS_WITH_SUBPROJECTS),)
internal-clean:: $(LIBRARYS_WITH_SUBPROJECTS:=.clean.library.subprojects)
endif

$(LIBRARY_NAME):
	$(ECHO_MAKE)$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) $@.all.library.variables

$(eval $(call __mod,master/library.mk))
