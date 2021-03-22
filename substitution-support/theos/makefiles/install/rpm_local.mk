internal-install:: internal-install-check
	$(ECHO_INSTALLING)install.exec "$(_THEOS_SUDO_COMMAND) rpm -U --replacepkgs --oldpackage \"$(_THEOS_PACKAGE_LAST_FILENAME)\""$(ECHO_END)

internal-uninstall::
	$(ECHO_NOTHING)install.exec "rpm -e \"$(THEOS_PACKAGE_NAME)\""$(ECHO_END)
