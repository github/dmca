package Logos::Generator::Base::Subclass;
use strict;

sub declarations {
	::fileError(-1, "generator does not implement Subclass::declarations");
}

sub initializers {
	::fileError(-1, "generator does not implement Subclass::initializers");
}

1;
