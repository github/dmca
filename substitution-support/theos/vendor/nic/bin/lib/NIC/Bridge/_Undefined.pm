package NIC::Bridge::_Undefined;
use strict;
use warnings;
use parent qw(NIC::Bridge::_BridgedObject);

use overload "bool" => sub {
	return 0;
};

our $AUTOLOAD;
sub AUTOLOAD {
	my $method = $AUTOLOAD;
	$method =~ s/.*:://;
	NIC::Bridge::Context::error("Method '$method' called on nonexistent NIC Bridge object.");
}

sub DESTROY {
	my $self = shift;
}

1;
