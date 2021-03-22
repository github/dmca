package NIC::Bridge::NICType;
use strict;
use warnings;
use parent qw(NIC::Bridge::_BridgedObject);
use NIC::Tie::Method;

sub name :lvalue {
	my $self = shift;
	tie my $tied, 'NIC::Tie::Method', $self->{FOR}, "name";
	$tied;
}

sub mode :lvalue {
	my $self = shift;
	tie my $tied, 'NIC::Tie::Method', $self->{FOR}, "mode";
	$tied;
}

sub constraints {
	my $self = shift;
	return $self->{FOR}->constraints;
}

sub constrain {
	my $self = shift;
	$self->{FOR}->addConstraint(shift);
}

1;
