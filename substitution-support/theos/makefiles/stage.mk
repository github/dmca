ifeq ($(_THEOS_STAGING_RULES_LOADED),)
_THEOS_STAGING_RULES_LOADED := 1

.PHONY: stage before-stage internal-stage after-stage

# For the toplevel invocation of make, mark 'all' and the *-stage rules as prerequisites.
# We do not do this for anything else, because otherwise, all the staging rules would run for every subproject.
ifeq ($(_THEOS_TOP_INVOCATION_DONE),)
stage:: all before-stage internal-stage after-stage
else # _THEOS_TOP_INVOCATION_DONE
stage:: internal-stage
endif

# Only do the master staging rules if we're the toplevel make invocation.
ifeq ($(_THEOS_TOP_INVOCATION_DONE),)
before-stage::
	$(ECHO_NOTHING)rm -rf "$(THEOS_STAGING_DIR)"$(ECHO_END)
	$(ECHO_NOTHING)$(FAKEROOT) -c$(ECHO_END)
	$(ECHO_NOTHING)mkdir -p "$(THEOS_STAGING_DIR)"$(ECHO_END)
else # _THEOS_TOP_INVOCATION_DONE
before-stage::
	@:
endif # _THEOS_TOP_INVOCATION_DONE

internal-stage::
	$(ECHO_NOTHING)[ -d $(THEOS_LAYOUT_DIR_NAME) ] && rsync -a "$(THEOS_LAYOUT_DIR_NAME)/" "$(THEOS_STAGING_DIR)" --exclude "DEBIAN" $(_THEOS_RSYNC_EXCLUDE_COMMANDLINE) || true$(ECHO_END)

after-stage::
	@:

endif # _THEOS_STAGING_RULES_LOADED
