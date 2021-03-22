package NIC::Bridge::File;
use strict;
use warnings;
use parent qw(NIC::Bridge::NICType);
use NIC::Tie::Method;

sub data :lvalue {
	my $self = shift;
	tie my $tied, 'NIC::Tie::Method', $self->{FOR}, "data";
	$tied;
}

1;
