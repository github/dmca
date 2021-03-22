internal-install:: internal-install-check
	$(ECHO_INSTALLING)install.exec "cat > /tmp/_theos_install.rpm; $(_THEOS_SUDO_COMMAND) rpm -U --replacepkgs --oldpackage /tmp/_theos_install.rpm && rm /tmp/_theos_install.rpm" < "$(_THEOS_PACKAGE_LAST_FILENAME)"$(ECHO_END)

internal-uninstall::
	$(ECHO_NOTHING)install.exec "$(_THEOS_SUDO_COMMAND) rpm -e \"$(THEOS_PACKAGE_NAME)\""$(ECHO_END)
