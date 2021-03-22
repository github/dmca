package Logos::Generator::Base::Group;
use strict;

sub declarations {
	my $self = shift;
	my $group = shift;
	my $return = "";
	foreach(@{$group->classes}) {
		$return .= Logos::Generator::for($_)->declarations if $_->initRequired;
	}
	return $return;
}

sub initializers {
	my $self = shift;
	my $group = shift;
	my $return = "{";
	foreach(@{$group->classes}) {
		$return .= Logos::Generator::for($_)->initializers if $_->initRequired;
	}
	foreach(@{$group->functions}) {
		$return .= Logos::Generator::for($_)->initializers if $_->initRequired;
	}
	$return .= "}";
	return $return;
}

1;
