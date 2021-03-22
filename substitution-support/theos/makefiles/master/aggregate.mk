ifeq ($(_THEOS_RULES_LOADED),)
include $(THEOS_MAKE_PATH)/rules.mk
endif

SUBPROJECTS := $(strip $(call __schema_var_all,,SUBPROJECTS))
ifneq ($(SUBPROJECTS),)
internal-all internal-clean:: _OPERATION = $(subst internal-,,$@)
internal-stage internal-after-install:: _OPERATION = $@
internal-all internal-clean internal-stage internal-after-install:: _OPERATION_NAME = $(subst internal-,,$@)

internal-all internal-clean internal-stage internal-after-install::
	+@abs_build_dir=$(_THEOS_ABSOLUTE_BUILD_DIR); \
	for d in $(SUBPROJECTS); do \
	  $(PRINT_FORMAT_MAKING) "Making $(_OPERATION_NAME) in $$d"; \
	  if [[ "$${abs_build_dir}" = "." ]]; then \
	    lbuilddir="."; \
	  else \
	    lbuilddir="$${abs_build_dir}/$$d"; \
	  fi; \
		if $(MAKE) -C $$d -f $(_THEOS_PROJECT_MAKEFILE_NAME) --no-keep-going $(_OPERATION) \
		THEOS_BUILD_DIR="$$lbuilddir" \
	     ; then\
	     :; \
	  else exit $$?; \
	  fi; \
	done;
endif

$(eval $(call __mod,master/aggregate.mk))
