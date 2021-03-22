internal-install:: internal-install-check
	$(ECHO_INSTALLING)install.exec "$(_THEOS_SUDO_COMMAND) dpkg -i \"$(_THEOS_PACKAGE_LAST_FILENAME)\""$(ECHO_END)

internal-uninstall::
	$(ECHO_NOTHING)install.exec "$(_THEOS_SUDO_COMMAND) dpkg -r \"$(THEOS_PACKAGE_NAME)\""$(ECHO_END)
