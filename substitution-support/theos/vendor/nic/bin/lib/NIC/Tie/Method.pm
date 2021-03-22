package NIC::Tie::Method;
use parent qw(Tie::Scalar);

sub TIESCALAR {
	my $class = shift;
	my $for = shift;
	my $method = shift;
	my $self = { FOR => $for, METHOD => $method };
	bless($self, $class);
	return $self;
}

sub FETCH {
	my $self = shift;
	my $obj = $self->{FOR};
	{ unshift @_, $obj; goto &{$self->{FOR}->can($self->{METHOD})}; }
}

sub STORE {
	my $self = shift;
	my $obj = $self->{FOR};
	{ unshift @_, $obj; goto &{$self->{FOR}->can($self->{METHOD})}; }
}

1;

