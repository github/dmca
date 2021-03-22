ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

.PHONY: internal-null-all_ internal-null-stage_

internal-null-all_::

internal-null-stage_::

$(eval $(call __mod,instance/null.mk))
