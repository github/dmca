package Logos::Property;
use strict;

##################### #
# Setters and Getters #
# #####################

sub new {
	my $proto = shift;
	my $class = ref($proto) || $proto;
	my $self = {};
	$self->{CLASS} = undef;
	$self->{GROUP} = undef;
	$self->{NAME} = undef;
	$self->{TYPE} = undef;
	$self->{ASSOCIATIONPOLICY} = undef;
	$self->{GETTER} = undef;
	$self->{SETTER} = undef;
	bless($self, $class);
	return $self;
}

sub class {
	my $self = shift;
	if(@_) { $self->{CLASS} = shift; }
	return $self->{CLASS};
}

sub group {
	my $self = shift;
	if(@_) { $self->{GROUP} = shift; }
	return $self->{GROUP};
}

sub name {
	my $self = shift;
	if(@_) { $self->{NAME} = shift; }
	return $self->{NAME};
}

sub type {
	my $self = shift;
	if(@_) { $self->{TYPE} = shift; }
	return $self->{TYPE};
}

sub associationPolicy {
	my $self = shift;
	if(@_) { $self->{ASSOCIATIONPOLICY} = shift; }
	return $self->{ASSOCIATIONPOLICY};
}

sub getter {
	my $self = shift;
	if(@_) { $self->{GETTER} = shift; }
	return $self->{GETTER};
}

sub setter {
	my $self = shift;
	if(@_) { $self->{SETTER} = shift; }
	return $self->{SETTER};
}

##### #
# END #
# #####

1;
