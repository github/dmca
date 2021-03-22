internal-install:: stage
	$(ECHO_INSTALLING)install.mergeDir "$(THEOS_STAGING_DIR)" "/"$(ECHO_END)

internal-uninstall::
	$(ERROR_BEGIN)"$(MAKE) uninstall is not supported when packaging is disabled"$(ERROR_END)
