package NIC::Bridge::Tie::WrappedMethod;
use strict;
use warnings;
use parent qw(Tie::Scalar);

sub TIESCALAR {
	my $class = shift;
	my $self = { CONTEXT => shift, FOR => shift, METHOD => shift };
	bless($self, $class);
	return $self;
}

sub FETCH {
	my $self = shift;
	my $obj = $self->{FOR};
	my $ret = $self->{FOR}->can($self->{METHOD})->($self->{FOR});
	return $self->{CONTEXT}->_wrap($ret);
}

sub STORE {
	my $self = shift;
	my $obj = $self->{FOR};
	my $in = shift;
	$self->{FOR}->can($self->{METHOD})->($self->{FOR}, $self->{CONTEXT}->_unwrap($in));
}

1;

