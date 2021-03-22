package Logos::Generator::internal::Function;
use strict;
use parent qw(Logos::Generator::Base::Function);

sub initializers {
	my $self = shift;
	my $function = shift;

	my $return = " /* internal::Function does not implement initializers */;";

	return $return;
}

1;
