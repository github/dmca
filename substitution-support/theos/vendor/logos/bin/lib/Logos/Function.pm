package Logos::Function;
use strict;

sub new {
	my $proto = shift;
	my $function = ref($proto) || $proto;
	my $self = {};
	$self->{NAME} = undef;
	$self->{RETVAL} = undef;
	$self->{ARGS} = [];
	$self->{GROUP} = undef;
	$self->{REQUIRED} = 0;
	bless($self, $function);
	return $self;
}

##################### #
# Setters and Getters #
# #####################
sub name {
	my $self = shift;
	if(@_) { $self->{NAME} = shift; }
	return $self->{NAME};
}

sub retval {
	my $self = shift;
	if(@_) { $self->{RETVAL} = shift; }
	return $self->{RETVAL};
}

sub args {
	my $self = shift;
	return $self->{ARGS};
}

sub group {
	my $self = shift;
	if(@_) { $self->{GROUP} = shift; }
	return $self->{GROUP};
}

sub required {
	my $self = shift;
	if(@_) { $self->{REQUIRED} = shift; }
	return $self->{REQUIRED};
}

sub initRequired {
	my $self = shift;
	return 1;
}

##### #
# END #
# #####

sub addArg {
	my $self = shift;
	my $arg = shift;
	push(@{$self->{ARGS}}, $arg);
}

1;
