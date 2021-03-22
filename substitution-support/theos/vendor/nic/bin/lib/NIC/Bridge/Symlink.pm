package NIC::Bridge::Symlink;
use strict;
use warnings;
use parent qw(NIC::Bridge::NICType);
use NIC::Bridge::Tie::WrappedMethod;

sub target :lvalue {
	my $self = shift;
	tie my $tied, 'NIC::Bridge::Tie::WrappedMethod', $self->{CONTEXT}, $self->{FOR}, "target";
	$tied;
}

1;
