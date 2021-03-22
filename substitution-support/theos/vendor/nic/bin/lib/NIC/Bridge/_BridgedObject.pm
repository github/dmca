package NIC::Bridge::_BridgedObject;
use strict;
use warnings;

use overload '""' => sub {
	my $self = shift;
	return "[".($self->{FOR}//"(undefined)")."]";
};

sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = { CONTEXT => shift, FOR => shift };
	bless($self, $proto);
	return $self;
}


1;
