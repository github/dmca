ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

# Unset codesign command line. No codesigning is involved here.
_THEOS_CODESIGN_COMMANDLINE =

.PHONY: internal-subproject-all_ internal-subproject-stage_ internal-subproject-compile

ifeq ($(_THEOS_MAKE_PARALLEL_BUILDING), no)
internal-subproject-all_:: $(_OBJ_DIR_STAMPS) $(THEOS_OBJ_DIR)/$(THEOS_CURRENT_INSTANCE).$(THEOS_SUBPROJECT_PRODUCT)
else
internal-subproject-all_:: $(_OBJ_DIR_STAMPS)
	$(ECHO_MAKE)$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) \
		internal-subproject-compile \
		_THEOS_CURRENT_TYPE=$(_THEOS_CURRENT_TYPE) THEOS_CURRENT_INSTANCE=$(THEOS_CURRENT_INSTANCE) _THEOS_CURRENT_OPERATION=compile \
		THEOS_BUILD_DIR="$(THEOS_BUILD_DIR)" _THEOS_MAKE_PARALLEL=yes

internal-subproject-compile: $(THEOS_OBJ_DIR)/$(THEOS_CURRENT_INSTANCE).$(THEOS_SUBPROJECT_PRODUCT)
endif

$(eval $(call _THEOS_TEMPLATE_DEFAULT_LINKING_RULE,$(THEOS_CURRENT_INSTANCE).$(THEOS_SUBPROJECT_PRODUCT)))

$(eval $(call __mod,instance/subproject.mk))
