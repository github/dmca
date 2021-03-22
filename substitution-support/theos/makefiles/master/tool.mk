TOOL_NAME := $(strip $(TOOL_NAME))

ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

internal-all:: $(TOOL_NAME:=.all.tool.variables);

internal-stage:: $(TOOL_NAME:=.stage.tool.variables);

TOOLS_WITH_SUBPROJECTS = $(strip $(foreach tool,$(TOOL_NAME),$(patsubst %,$(tool),$(call __schema_var_all,$(tool)_,SUBPROJECTS))))
ifneq ($(TOOLS_WITH_SUBPROJECTS),)
internal-clean:: $(TOOLS_WITH_SUBPROJECTS:=.clean.tool.subprojects)
endif

$(TOOL_NAME):
	$(ECHO_MAKE)$(MAKE) -f $(_THEOS_PROJECT_MAKEFILE_NAME) $(_THEOS_MAKEFLAGS) $@.all.tool.variables

$(eval $(call __mod,master/tool.mk))
